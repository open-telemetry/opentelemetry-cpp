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

// #if defined __has_include
// #  if __has_include("TraceLoggingDynamic.h")

#define HAVE_ETW_PROVIDER

#    ifndef WIN32_LEAN_AND_MEAN
#      define WIN32_LEAN_AND_MEAN
#    endif

#    ifdef _MSC_VER
// evntprov.h(838) : warning C4459 : declaration of 'Version' hides global declaration
#      pragma warning(disable : 4459)
// needed for Unit Testing with krabs.hpp
#      pragma warning(disable : 4018)
#    endif

#    include <opentelemetry/common/attribute_value.h>
#    include <opentelemetry/version.h>
#    include <opentelemetry/event/UUID.hpp>

#    include "utils.hpp"

#    include "TraceLoggingDynamic.h"

#    include <map>
#    include <mutex>
#    include <unordered_map>
#    include <vector>

#    ifdef HAVE_KRABS_TESTS
// krabs.hpp requires this definition of min macro from Windows.h
#      ifndef min
#        define min(a, b) (((a) < (b)) ? (a) : (b))
#      endif
#    endif

#    define MICROSOFT_EVENTTAG_NORMAL_PERSISTENCE 0x01000000

// using namespace tld;

OPENTELEMETRY_BEGIN_NAMESPACE

class ETWProvider
{

public:

  const unsigned long STATUS_OK = 0;
  const unsigned long STATUS_ERROR = ULONG_MAX;
  const unsigned long STATUS_EFBIG = ULONG_MAX - 1;

  /// <summary>
  /// Entry that contains Provider Handle, Provider MetaData and Provider GUID
  /// </summary>
  struct Handle
  {
    uint64_t providerHandle;
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
  Handle &open(const std::string &providerId)
  {
    std::lock_guard<std::mutex> lock(m_providerMapLock);

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

    event::UUID guid = (providerId.rfind("{", 0)==0) ? event::UUID(providerId.c_str())
                                                     :                  // It's a ProviderGUID
                           utils::GetProviderGuid(providerId.c_str());  // It's a ProviderName

    data.providerGuid = guid.to_GUID();

    // TODO: currently we do not allow to specify a custom group GUID
    GUID providerGroupGuid = NULL_GUID;

    tld::ProviderMetadataBuilder<std::vector<BYTE>> providerMetaBuilder(data.providerMetaVector);

    // Use Tenant ID as provider Name
    providerMetaBuilder.Begin(providerId.c_str());
    providerMetaBuilder.AddTrait(tld::ProviderTraitType::ProviderTraitGroupGuid,
                                 (void *)&providerGroupGuid, sizeof(GUID));
    providerMetaBuilder.End();

    REGHANDLE hProvider = 0;
    if (0 != tld::RegisterProvider(&hProvider, &data.providerGuid, data.providerMetaVector.data()))
    {
      data.providerHandle = INVALID_HANDLE;
    }
    else
    {
      data.providerHandle = hProvider;
    };

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

    auto m = providers();
    auto it = m.begin();
    while (it!=m.end())
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

  /// <summary>
  /// Send event to Provider Id
  /// </summary>
  /// <param name="providerId"></param>
  /// <param name="eventData"></param>
  /// <returns></returns>
  template<class T>
  unsigned long write(Handle &providerData, T eventData)
  {
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
    auto eventName               = nostd::get<const char *>(eventData[EVENT_NAME]);

    builder.Begin(eventName, eventTags);

    for (auto &kv : eventData)
    {
      const char *name = kv.first.data();
      // Don't include event name field in the payload
      if (EVENT_NAME == name)
        continue;
      auto &value      = kv.second;
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
#if 0
        case common::AttributeType::TYPE_CSTRING: {
          builder.AddField(name, tld::TypeUtf8String);
          auto temp = nostd::get<const char *>(value);
          dbuilder.AddString(temp);
          break;
        }
#endif

#    if HAVE_TYPE_GUID
          // TODO: consider adding UUID/GUID to spec
        case common::AttributeType::TYPE_GUID: {
          builder.AddField(name.c_str(), TypeGuid);
          auto temp = nostd::get<GUID>(value);
          dbuilder.AddBytes(&temp, sizeof(GUID));
          break;
        }
#    endif

        // TODO: arrays are not supported
#if 0
        case common::AttributeType::TYPE_SPAN_BYTE:
#endif
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
  }

protected:

  const unsigned int LargeEventSizeKB = 62;

  const REGHANDLE INVALID_HANDLE = _UI64_MAX;

  const GUID NULL_GUID = {0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

  mutable std::mutex m_providerMapLock;

  using ProviderMap = std::map<std::string, Handle>;

  ProviderMap& providers()
  {
    static std::map<std::string, Handle> providers;
    return providers;
  };

};

OPENTELEMETRY_END_NAMESPACE

// #  endif
// #endif