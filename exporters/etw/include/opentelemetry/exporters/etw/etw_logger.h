// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#include <algorithm>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <fstream>

#include <map>


#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"

#include "opentelemetry/common/key_value_iterable_view.h"

#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/logs/logger_provider.h"

#include "opentelemetry/exporters/etw/etw_fields.h"
#include "opentelemetry/exporters/etw/etw_properties.h"
#include "opentelemetry/exporters/etw/etw_provider.h"
#include "opentelemetry/exporters/etw/utils.h"

    namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace etw
{

/**
 * @brief TracerProvider Options passed via SDK API.
 */
using LoggerProviderOptions =
    std::map<std::string, nostd::variant<std::string, uint64_t, float, bool>>;

/**
 * @brief LoggerProvider runtime configuration class. Internal representation
 * of LoggerProviderOptions used by various components of SDK.
 */
typedef struct
{
  ETWProvider::EventFormat
      encoding;  // Event encoding to use for this provider (TLD, MsgPack, XML, etc.).
} LoggerProviderConfiguration;

/**
 * @brief Helper template to convert a variant value from TracerProviderOptions to
 * LoggerProviderConfiguration
 *
 * @param options           TracerProviderOptions passed on API surface
 * @param key               Option name
 * @param value             Reference to destination value
 * @param defaultValue      Default value if option is not supplied
 */
template <typename T>
static inline void GetOption(const LoggerProviderOptions &options,
                             const char *key,
                             T &value,
                             T defaultValue)
{
  auto it = options.find(key);
  if (it != options.end())
  {
    auto val = it->second;
    value    = nostd::get<T>(val);
  }
  else
  {
    value = defaultValue;
  }
}

/**
 * @brief Helper template to convert encoding config option to EventFormat.
 * Configuration option passed as `options["encoding"] = "MsgPack"`.
 * Default encoding is TraceLogging Dynamic Manifest (TLD).
 *
 * Valid encoding names listed below.
 *
 * For MessagePack encoding:
 * - "MSGPACK"
 * - "MsgPack"
 * - "MessagePack"
 *
 * For XML encoding:
 * - "XML"
 * - "xml"
 *
 * For TraceLogging Dynamic encoding:
 * - "TLD"
 * - "tld"
 *
 */
static inline ETWProvider::EventFormat GetEncoding(const LoggerProviderOptions &options)
{
  ETWProvider::EventFormat evtFmt = ETWProvider::EventFormat::ETW_MANIFEST;

  auto it = options.find("encoding");
  if (it != options.end())
  {
    auto varValue   = it->second;
    std::string val = nostd::get<std::string>(varValue);

#pragma warning(push)
#pragma warning(disable : 4307) /* Integral constant overflow - OK while computing hash */
    auto h = utils::hashCode(val.c_str());
    switch (h)
    {
      case CONST_HASHCODE(MSGPACK):
        // nobrk
      case CONST_HASHCODE(MsgPack):
        // nobrk
      case CONST_HASHCODE(MessagePack):
        evtFmt = ETWProvider::EventFormat::ETW_MSGPACK;
        break;

      case CONST_HASHCODE(XML):
        // nobrk
      case CONST_HASHCODE(xml):
        evtFmt = ETWProvider::EventFormat::ETW_XML;
        break;

      case CONST_HASHCODE(TLD):
        // nobrk
      case CONST_HASHCODE(tld):
        // nobrk
        evtFmt = ETWProvider::EventFormat::ETW_MANIFEST;
        break;

      default:
        break;
    }
#pragma warning(pop)
  }

  return evtFmt;
}

/**
 * @brief Utility template to convert SpanId or TraceId to hex.
 * @param id - value of SpanId or TraceId
 * @return Hexadecimal representation of Id as string.
 */
template <class T>
static inline std::string ToLowerBase16(const T &id)
{
  char buf[2 * T::kSize] = {0};
  id.ToLowerBase16(buf);
  return std::string(buf, sizeof(buf));
}


class LoggerProvider;

/**
 * @brief Logger  class that allows to send logs to ETW Provider.
 */
class Logger : public logs::Logger
{

  /**
   * @brief Parent provider of this Tracer
   */
  etw::LoggerProvider &loggerProvider_;

  /**
   * @brief ProviderId (Name or GUID)
   */
  std::string provId;

  /**
   * @brief Encoding (Manifest, MessagePack or XML)
   */
  ETWProvider::EventFormat encoding;

  /**
   * @brief Provider Handle
   */
  ETWProvider::Handle &provHandle;

  /**
   * @brief ETWProvider is a singleton that aggregates all ETW writes.
   * @return
   */
  static ETWProvider &etwProvider()
  {
    static ETWProvider instance;  // C++11 magic static
    return instance;
  };

  /**
   * @brief Init a reference to etw::ProviderHandle
   * @return Provider Handle
   */
  ETWProvider::Handle &initProvHandle()
  {
    return etwProvider().open(provId, encoding);
  }

public:
  /**
   * @brief Tracer constructor
   * @param parent Parent LoggerProvider
   * @param providerId ProviderId - Name or GUID
   * @param encoding ETW encoding format to use.
   */
  Logger(etw::LoggerProvider &parent,
         nostd::string_view providerId     = "",
         ETWProvider::EventFormat encoding = ETWProvider::EventFormat::ETW_MANIFEST)
      : logs::Logger(),
        loggerProvider_(parent),
        provId(providerId.data(), providerId.size()),
        encoding(encoding),
        provHandle(initProvHandle())
  {}

  void Log(logs::Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const common::KeyValueIterable &resource,
           const common::KeyValueIterable &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept override
  {

#ifdef RTTI_ENABLED
    common::KeyValueIterable &attribs = const_cast<common::KeyValueIterable &>(attributes);
    // common::KeyValueIterable &resr = const_cast<common::KeyValueIterable &>(resource);
    Properties *evt = dynamic_cast<Properties *>(&attribs);
    // Properties *res                   = dynamic_cast<Properties *>(&resr);

    if (evt != nullptr)
    {
      // Pass as a reference to original modifyable collection without creating a copy
      return Log(severity, name, body, *evt, trace_id, span_id, trace_flags, timestamp);
    }
#endif
    Properties evtCopy = attributes;
    //Properties resCopy = resource;
    return Log(severity, name, body, evtCopy, trace_id, span_id, trace_flags, timestamp);
  }

  virtual void Log(logs::Severity severity,
                   nostd::string_view name,
                   nostd::string_view body,
                   Properties &evt,
                   trace::TraceId trace_id,
                   trace::SpanId span_id,
                   trace::TraceFlags trace_flags,
                   common::SystemTimestamp timestamp) noexcept
  {
    // Populate Etw.EventName attribute at envelope level
    evt[ETW_FIELD_NAME] = ETW_VALUE_LOG;

#ifdef HAVE_FIELD_TIME
    {
      auto timeNow        = std::chrono::system_clock::now().time_since_epoch();
      auto millis         = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow).count();
      evt[ETW_FIELD_TIME] = utils::formatUtcTimestampMsAsISO8601(millis);
    }
#endif

    evt[ETW_FIELD_SPAN_ID]                   = ToLowerBase16(span_id);
    evt[ETW_FIELD_TRACE_ID]                  = ToLowerBase16(trace_id);
    evt[ETW_FIELD_PAYLOAD_NAME]              = std::string(name.data(), name.length());
    std::chrono::system_clock::time_point ts = timestamp;
    int64_t tsMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(ts.time_since_epoch()).count();
    evt[ETW_FIELD_TIMESTAMP] = utils::formatUtcTimestampMsAsISO8601(tsMs);
    evt[ETW_FIELD_LOG_SEVERITY_TEXT] =
        opentelemetry::logs::SeverityNumToText[static_cast<int>(severity)].data();
    evt[ETW_FIELD_LOG_SEVERITY_NUM] = static_cast<uint32_t>(severity);
    evt[ETW_FIELD_LOG_BODY]         = std::string(body.data(), body.length());
    etwProvider().write(provHandle, evt, nullptr, nullptr, 0, encoding);
  }

  const nostd::string_view GetName() noexcept override { return std::string();}
    // TODO : Flush and Shutdown method in main Tracer API
  ~Logger() {
      etwProvider().close(provHandle);
  }
};

/**
 * @brief ETW LoggerProvider
 */
class LoggerProvider : public logs::LoggerProvider
{
public:
  /**
   * @brief LoggerrProvider options supplied during initialization.
   */
  LoggerProviderConfiguration config_;

  /**
   * @brief Construct instance of TracerProvider with given options
   * @param options Configuration options
   */
  LoggerProvider(LoggerProviderOptions options) : logs::LoggerProvider()
  {

    // Determines what encoding to use for ETW events: TraceLogging Dynamic, MsgPack, XML, etc.
    config_.encoding = GetEncoding(options);
  }

  LoggerProvider() : logs::LoggerProvider()
  {
    config_.encoding                = ETWProvider::EventFormat::ETW_MANIFEST;
  }

  /**
   * @brief Obtain ETW Tracer.
   * @param name ProviderId (instrumentation name) - Name or GUID
   *
   * @param args Additional arguments that controls `codec` of the provider.
   * Possible values are:
   * - "ETW"            - 'classic' Trace Logging Dynamic manifest ETW events.
   * - "MSGPACK"        - MessagePack-encoded binary payload ETW events.
   * - "XML"            - XML events (reserved for future use)
   * @return
   */
  nostd::shared_ptr<logs::Logger> GetLogger(nostd::string_view name,
                                             nostd::string_view args       = "") override
  {
    UNREFERENCED_PARAMETER(args);
    ETWProvider::EventFormat evtFmt = config_.encoding;
    return nostd::shared_ptr<logs::Logger>{new (std::nothrow) etw::Logger(*this, name, evtFmt)};
  }

  nostd::shared_ptr<logs::Logger> GetLogger(nostd::string_view name,
                                              nostd::span<nostd::string_view> args) override
  {
      return GetLogger(name, args[0]);

  }
};

}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif // ENABLE_LOGS_PREVIEW
