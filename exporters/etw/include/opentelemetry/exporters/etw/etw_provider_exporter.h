// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#ifdef _MSC_VER
// evntprov.h(838) : warning C4459 : declaration of 'Version' hides global declaration
#  pragma warning(disable : 4459)
// needed for Unit Testing with krabs.hpp
#  pragma warning(disable : 4018)
#endif

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/exporters/etw/uuid.h"
#include "opentelemetry/version.h"

#include "opentelemetry/exporters/etw/utils.h"

#ifdef HAVE_MSGPACK
// This option requires INCLUDE_DIR=$(ProjectDir)\..\..\third_party\json\include;...
#  include "nlohmann/json.hpp"
#endif

#ifndef HAVE_NO_TLD
// Allow to opt-out from `TraceLoggingDynamic.h` header usage
#  define HAVE_TLD
#  include "TraceLoggingDynamic.h"
#endif

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

#ifdef HAVE_KRABS_TESTS
// krabs.hpp requires this definition of min macro from Windows.h
#  ifndef min
#    define min(a, b) (((a) < (b)) ? (a) : (b))
#  endif
#endif

#define MICROSOFT_EVENTTAG_NORMAL_PERSISTENCE 0x01000000

OPENTELEMETRY_BEGIN_NAMESPACE

class ETWProvider
{

public:
  const unsigned long STATUS_OK    = 0;
  const unsigned long STATUS_ERROR = ULONG_MAX;
  const unsigned long STATUS_EFBIG = ULONG_MAX - 1;

  enum EventFormat
  {
    ETW_MANIFEST = 0,
    ETW_MSGPACK  = 1,
    ETW_XML      = 2
  };

  /// <summary>
  /// Entry that contains Provider Handle, Provider MetaData and Provider GUID
  /// </summary>
  struct Handle
  {
    REGHANDLE providerHandle;
    std::vector<BYTE> providerMetaVector;
    GUID providerGuid;
  };

  /// <summary>
  /// Check if given provider is registered.
  /// </summary>
  /// <param name="providerId"></param>
  /// <returns></returns>
  bool is_registered(const std::string &providerId)
  {
    std::lock_guard<std::mutex> lock(m_providerMapLock);
    auto it = providers().find(providerId);
    if (it != providers().end())
    {
      if (it->second.providerHandle != INVALID_HANDLE)
      {
        return true;
      }
    }
    return false;
  }

  /// <summary>
  /// Get Provider by Name or string representation of GUID
  /// </summary>
  /// <param name="providerId"></param>
  /// <returns></returns>
  Handle &open(const std::string &providerId, EventFormat format = EventFormat::ETW_MANIFEST)
  {
    std::lock_guard<std::mutex> lock(m_providerMapLock);

#ifdef HAVE_NO_TLD
    // Fallback to MessagePack-encoded ETW events
    format = EventFormat::ETW_MSGPACK;
#endif

    // Check and return if provider is already registered
    auto it = providers().find(providerId);
    if (it != providers().end())
    {
      if (it->second.providerHandle != INVALID_HANDLE)
      {
        return it->second;
      }
    }

    // Register provider if necessary
    auto &data = providers()[providerId];
    data.providerMetaVector.clear();

    utils::UUID guid = (providerId.rfind("{", 0) == 0) ? utils::UUID(providerId.c_str())
                                                       :                // It's a ProviderGUID
                           utils::GetProviderGuid(providerId.c_str());  // It's a ProviderName

    data.providerGuid = guid.to_GUID();

    // TODO: currently we do not allow to specify a custom group GUID
    GUID providerGroupGuid = NULL_GUID;

    switch (format)
    {
#ifdef HAVE_TLD
      // Register with TraceLoggingDynamic facility - dynamic manifest ETW events.
      case EventFormat::ETW_MANIFEST: {
        tld::ProviderMetadataBuilder<std::vector<BYTE>> providerMetaBuilder(
            data.providerMetaVector);

        // Use Tenant ID as provider Name
        providerMetaBuilder.Begin(providerId.c_str());
        providerMetaBuilder.AddTrait(tld::ProviderTraitType::ProviderTraitGroupGuid,
                                     (void *)&providerGroupGuid, sizeof(GUID));
        providerMetaBuilder.End();

        REGHANDLE hProvider = 0;
        if (0 !=
            tld::RegisterProvider(&hProvider, &data.providerGuid, data.providerMetaVector.data()))
        {
          // There was an error registering the ETW provider
          data.providerHandle = INVALID_HANDLE;
        }
        else
        {
          data.providerHandle = hProvider;
        };
      };
      break;
#endif

#ifdef HAVE_MSGPACK
      // Register for MsgPack payload ETW events.
      case EventFormat::ETW_MSGPACK: {
        REGHANDLE hProvider = 0;
        if (EventRegister(&data.providerGuid, NULL, NULL, &hProvider) != ERROR_SUCCESS)
        {
          // There was an error registering the ETW provider
          data.providerHandle = INVALID_HANDLE;
        }
        else
        {
          data.providerHandle = hProvider;
        }
      };
      break;
#endif

      default:
        // TODO: other protocols, e.g. XML events - not supported yet
        break;
    }

    // We always return an entry even if we failed to register.
    // Caller should check whether the hProvider handle is valid.
    return data;
  }

  /// <summary>
  /// Unregister Provider
  /// </summary>
  /// <param name="providerId"></param>
  /// <returns></returns>
  unsigned long close(Handle data)
  {
    std::lock_guard<std::mutex> lock(m_providerMapLock);

    auto m  = providers();
    auto it = m.begin();
    while (it != m.end())
    {
      if (it->second.providerHandle == data.providerHandle)
      {
        auto result = EventUnregister(data.providerHandle);
        m.erase(it);
        return result;
      }
    };
    return STATUS_ERROR;
  }

#ifdef HAVE_MSGPACK
  template <class T>
  unsigned long writeMsgPack(Handle &providerData, T eventData)
  {

    // Make sure you stop sending event before register unregistering providerData
    if (providerData.providerHandle == INVALID_HANDLE)
    {
      // Provider not registered!
      return STATUS_ERROR;
    };

    const std::string EVENT_NAME = "name";
    std::string eventName        = "NoName";
    auto nameField               = eventData[EVENT_NAME];
    switch (nameField.index())
    {
      case common::AttributeType::TYPE_STRING:
        eventName =
            (char *)(nostd::get<nostd::string_view>(nameField).data());  // must be 0-terminated!
        break;
#  ifdef HAVE_CSTRING_TYPE
      case common::AttributeType::TYPE_CSTRING:
        eventName = (char *)(nostd::get<const char *>(nameField));
        break;
#  endif
      default:
        // This is user error. Invalid event name!
        // We supply default 'NoName' event name in this case.
        break;
    }

    /* clang-format off */
    nlohmann::json jObj =
    {
      { "env_name", "Span" },
      { "env_ver", "4.0" },
      //
      // TODO: compute time in MessagePack-friendly format
      // TODO: should we consider uint64_t format with Unix timestamps for ELK stack?
      { "env_time",
        {
          { "TypeCode", 255 },
          { "Body","0xFFFFFC60000000005F752C2C" }
        }
      },
      //

      // TODO: follow JSON implementation of OTLP or place protobuf for non-Microsoft flows
      { "env_dt_traceId", "6dcdae7b9b0c7643967d74ee54056178" },
      { "env_dt_spanId", "5866c4322919e641" },
      //
      { "name", eventName },
      { "kind", 0 },
      { "startTime",
        {
          // TODO: timestamp
          { "TypeCode", 255 },
          { "Body", "0xFFFF87CC000000005F752C2C" }
        }
      }
    };
    /* clang-format on */

    for (auto &kv : eventData)
    {
      const char *name = kv.first.data();
      // Don't include event name field in the payload
      if (EVENT_NAME == name)
        continue;
      auto &value = kv.second;
      switch (value.index())
      {
        case common::AttributeType::TYPE_BOOL: {
          UINT8 temp = static_cast<UINT8>(nostd::get<bool>(value));
          jObj[name] = temp;
          break;
        }
        case common::AttributeType::TYPE_INT: {
          auto temp  = nostd::get<int32_t>(value);
          jObj[name] = temp;
          break;
        }
        case common::AttributeType::TYPE_INT64: {
          auto temp  = nostd::get<int64_t>(value);
          jObj[name] = temp;
          break;
        }
        case common::AttributeType::TYPE_UINT: {
          auto temp  = nostd::get<uint32_t>(value);
          jObj[name] = temp;
          break;
        }
        case common::AttributeType::TYPE_UINT64: {
          auto temp  = nostd::get<uint64_t>(value);
          jObj[name] = temp;
          break;
        }
        case common::AttributeType::TYPE_DOUBLE: {
          auto temp  = nostd::get<double>(value);
          jObj[name] = temp;
          break;
        }
        case common::AttributeType::TYPE_STRING: {
          auto temp  = nostd::get<nostd::string_view>(value);
          jObj[name] = temp;
          break;
        }
#  ifdef HAVE_CSTRING_TYPE
        case common::AttributeType::TYPE_CSTRING: {
          auto temp  = nostd::get<const char *>(value);
          jObj[name] = temp;
          break;
        }
#  endif
#  if HAVE_TYPE_GUID
          // TODO: consider adding UUID/GUID to spec
        case common::AttributeType::TYPE_GUID: {
          auto temp = nostd::get<GUID>(value);
          // TODO: add transform from GUID type to string?
          jObj[name] = temp;
          break;
        }
#  endif
        // TODO: arrays are not supported yet
#  ifdef HAVE_SPAN_BYTE
        case common::AttributeType::TYPE_SPAN_BYTE:
#  endif
        case common::AttributeType::TYPE_SPAN_BOOL:
        case common::AttributeType::TYPE_SPAN_INT:
        case common::AttributeType::TYPE_SPAN_INT64:
        case common::AttributeType::TYPE_SPAN_UINT:
        case common::AttributeType::TYPE_SPAN_UINT64:
        case common::AttributeType::TYPE_SPAN_DOUBLE:
        case common::AttributeType::TYPE_SPAN_STRING:
        default:
          // TODO: unsupported type
          break;
      }
    };

    // Layer 1
    nlohmann::json l1 = nlohmann::json::array();
    // Layer 2
    nlohmann::json l2 = nlohmann::json::array();
    // Layer 3
    nlohmann::json l3 = nlohmann::json::array();

    l1.push_back("Span");

    {
      // TODO: clarify why this is needed
      // TODO: fix time here
      nlohmann::json j;
      j["TypeCode"] = 255;
      j["Body"]     = "0xFFFFFC60000000005F752C2C";
      l3.push_back(j);
    };

    // Actual value object goes here
    l3.push_back(jObj);

    l2.push_back(l3);
    l1.push_back(l2);

    {
      // Another time field again, but at the top
      // TODO: fix time here
      nlohmann::json j;
      j["TypeCode"] = 255;
      j["Body"]     = "0xFFFFFC60000000005F752C2C";
      l1.push_back(j);
    };

    std::vector<uint8_t> v = nlohmann::json::to_msgpack(l1);

    EVENT_DESCRIPTOR evtDescriptor;
    EventDescCreate(&evtDescriptor, 0, 0x1, 0, 0, 0, 0, 0);
    EVENT_DATA_DESCRIPTOR evtData[1];
    EventDataDescCreate(&evtData[0], v.data(), static_cast<ULONG>(v.size()));

    auto writeResponse = EventWrite(providerData.providerHandle, &evtDescriptor, 1, evtData);

    switch (writeResponse)
    {
      case ERROR_INVALID_PARAMETER:
        break;
      case ERROR_INVALID_HANDLE:
        break;
      case ERROR_ARITHMETIC_OVERFLOW:
        break;
      case ERROR_MORE_DATA:
        break;
      case ERROR_NOT_ENOUGH_MEMORY:
        break;
      default:
        break;
    };

    if (writeResponse == ERROR_ARITHMETIC_OVERFLOW)
    {
      return STATUS_EFBIG;
    };
    return (unsigned long)(writeResponse);
  }
#endif

  /// <summary>
  /// Send event to Provider Id
  /// </summary>
  /// <param name="providerId"></param>
  /// <param name="eventData"></param>
  /// <returns></returns>
  template <class T>
  unsigned long writeTld(Handle &providerData, T eventData)
  {
#ifdef HAVE_TLD
    // Make sure you stop sending event before register unregistering providerData
    if (providerData.providerHandle == INVALID_HANDLE)
    {
      // Provider not registered!
      return STATUS_ERROR;
    };

    UINT32 eventTags = MICROSOFT_EVENTTAG_NORMAL_PERSISTENCE;

    std::vector<BYTE> byteVector;
    std::vector<BYTE> byteDataVector;
    tld::EventMetadataBuilder<std::vector<BYTE>> builder(byteVector);
    tld::EventDataBuilder<std::vector<BYTE>> dbuilder(byteDataVector);

    const std::string EVENT_NAME = "name";
    std::string eventName        = "NoName";
    auto nameField               = eventData[EVENT_NAME];
    switch (nameField.index())
    {
      case common::AttributeType::TYPE_STRING:
        eventName =
            (char *)(nostd::get<nostd::string_view>(nameField).data());  // must be 0-terminated!
        break;
#  ifdef HAVE_CSTRING_TYPE
      case common::AttributeType::TYPE_CSTRING:
        eventName = (char *)(nostd::get<const char *>(nameField));
        break;
#  endif
      default:
        // This is user error. Invalid event name!
        // We supply default 'NoName' event name in this case.
        break;
    }

    builder.Begin(eventName.c_str(), eventTags);

    for (auto &kv : eventData)
    {
      const char *name = kv.first.data();
      // Don't include event name field in the payload
      if (EVENT_NAME == name)
        continue;
      auto &value = kv.second;
      switch (value.index())
      {
        case common::AttributeType::TYPE_BOOL: {
          builder.AddField(name, tld::TypeBool8);
          UINT8 temp = static_cast<UINT8>(nostd::get<bool>(value));
          dbuilder.AddByte(temp);
          break;
        }
        case common::AttributeType::TYPE_INT: {
          builder.AddField(name, tld::TypeInt32);
          auto temp = nostd::get<int32_t>(value);
          dbuilder.AddValue(temp);
          break;
        }
        case common::AttributeType::TYPE_INT64: {
          builder.AddField(name, tld::TypeInt64);
          auto temp = nostd::get<int64_t>(value);
          dbuilder.AddValue(temp);
          break;
        }
        case common::AttributeType::TYPE_UINT: {
          builder.AddField(name, tld::TypeUInt32);
          auto temp = nostd::get<uint32_t>(value);
          dbuilder.AddValue(temp);
          break;
        }
        case common::AttributeType::TYPE_UINT64: {
          builder.AddField(name, tld::TypeUInt64);
          auto temp = nostd::get<uint64_t>(value);
          dbuilder.AddValue(temp);
          break;
        }
        case common::AttributeType::TYPE_DOUBLE: {
          builder.AddField(name, tld::TypeDouble);
          auto temp = nostd::get<double>(value);
          dbuilder.AddValue(temp);
          break;
        }
        case common::AttributeType::TYPE_STRING: {
          builder.AddField(name, tld::TypeUtf8String);
          auto temp = nostd::get<nostd::string_view>(value);
          dbuilder.AddString(temp.data());
          break;
        }
#  ifdef HAVE_CSTRING_TYPE
        case common::AttributeType::TYPE_CSTRING: {
          builder.AddField(name, tld::TypeUtf8String);
          auto temp = nostd::get<const char *>(value);
          dbuilder.AddString(temp);
          break;
        }
#  endif
#  if HAVE_TYPE_GUID
          // TODO: consider adding UUID/GUID to spec
        case common::AttributeType::TYPE_GUID: {
          builder.AddField(name.c_str(), TypeGuid);
          auto temp = nostd::get<GUID>(value);
          dbuilder.AddBytes(&temp, sizeof(GUID));
          break;
        }
#  endif

        // TODO: arrays are not supported
#  ifdef HAVE_SPAN_BYTE
        case common::AttributeType::TYPE_SPAN_BYTE:
#  endif
        case common::AttributeType::TYPE_SPAN_BOOL:
        case common::AttributeType::TYPE_SPAN_INT:
        case common::AttributeType::TYPE_SPAN_INT64:
        case common::AttributeType::TYPE_SPAN_UINT:
        case common::AttributeType::TYPE_SPAN_UINT64:
        case common::AttributeType::TYPE_SPAN_DOUBLE:
        case common::AttributeType::TYPE_SPAN_STRING:
        default:
          // TODO: unsupported type
          break;
      }
    };

    if (!builder.End())  // Returns false if the metadata is too large.
    {
      return STATUS_EFBIG;  // if event is too big for UTC to handle
    }

    tld::EventDescriptor eventDescriptor;
    // eventDescriptor.Keyword = MICROSOFT_KEYWORD_CRITICAL_DATA;
    // eventDescriptor.Keyword = MICROSOFT_KEYWORD_TELEMETRY;
    // eventDescriptor.Keyword = MICROSOFT_KEYWORD_MEASURES;

    EVENT_DATA_DESCRIPTOR pDataDescriptors[3];

    EventDataDescCreate(&pDataDescriptors[2], byteDataVector.data(),
                        static_cast<ULONG>(byteDataVector.size()));

    // Event size detection is needed
    int64_t eventByteSize = byteDataVector.size() + byteVector.size();
    int64_t eventKBSize   = (eventByteSize + 1024 - 1) / 1024;
    // bool isLargeEvent     = eventKBSize >= LargeEventSizeKB;

    // TODO: extract
    // - GUID ActivityId
    // - GUID RelatedActivityId

    HRESULT writeResponse = tld::WriteEvent(providerData.providerHandle, eventDescriptor,
                                            providerData.providerMetaVector.data(),
                                            byteVector.data(), 3, pDataDescriptors);

    if (writeResponse == HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW))
    {
      return STATUS_EFBIG;
    };

    return (unsigned long)(writeResponse);
#else
    return STATUS_ERROR;
#endif
  }

  template <class T>
  unsigned long write(Handle &providerData,
                      T eventData,
                      ETWProvider::EventFormat format = ETWProvider::EventFormat::ETW_MANIFEST)
  {
    if (format == ETWProvider::EventFormat::ETW_MANIFEST)
    {
      return writeTld(providerData, eventData);
    }
    if (format == ETWProvider::EventFormat::ETW_MSGPACK)
    {
      return writeMsgPack(providerData, eventData);
    }
    if (format == ETWProvider::EventFormat::ETW_XML)
    {
      // TODO: not implemented
      return STATUS_ERROR;
    }
    return STATUS_ERROR;
  };

  static const REGHANDLE INVALID_HANDLE = _UI64_MAX;

protected:
  const unsigned int LargeEventSizeKB = 62;

  const GUID NULL_GUID = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

  mutable std::mutex m_providerMapLock;

  using ProviderMap = std::map<std::string, Handle>;

  ProviderMap &providers()
  {
    static std::map<std::string, Handle> providers;
    return providers;
  };
};

OPENTELEMETRY_END_NAMESPACE
