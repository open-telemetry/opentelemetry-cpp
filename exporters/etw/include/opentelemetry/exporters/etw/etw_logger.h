// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <algorithm>

#  include <cstdint>
#  include <cstdio>
#  include <cstdlib>
#  include <sstream>
#  include <type_traits>

#  include <fstream>

#  include <map>

#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/unique_ptr.h"
#  include "opentelemetry/nostd/variant.h"

#  include "opentelemetry/common/key_value_iterable_view.h"

#  include "opentelemetry/logs/logger_provider.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_id.h"

#  include "opentelemetry/exporters/etw/etw_config.h"
#  include "opentelemetry/exporters/etw/etw_fields.h"
#  include "opentelemetry/exporters/etw/etw_properties.h"
#  include "opentelemetry/exporters/etw/etw_provider.h"
#  include "opentelemetry/exporters/etw/utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace etw
{

class LoggerProvider;

/**
 * @brief Logger  class that allows to send logs to ETW Provider.
 */
class Logger : public opentelemetry::logs::Logger
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
  }

  /**
   * @brief Init a reference to etw::ProviderHandle
   * @return Provider Handle
   */
  ETWProvider::Handle &initProvHandle() { return etwProvider().open(provId, encoding); }

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
      : opentelemetry::logs::Logger(),
        loggerProvider_(parent),
        provId(providerId.data(), providerId.size()),
        encoding(encoding),
        provHandle(initProvHandle())
  {}

  void Log(opentelemetry::logs::Severity severity,
           nostd::string_view body,
           const common::KeyValueIterable &attributes,
           opentelemetry::trace::TraceId trace_id,
           opentelemetry::trace::SpanId span_id,
           opentelemetry::trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept override
  {

#  ifdef OPENTELEMETRY_RTTI_ENABLED
    common::KeyValueIterable &attribs = const_cast<common::KeyValueIterable &>(attributes);
    Properties *evt                   = dynamic_cast<Properties *>(&attribs);
    // Properties *res                   = dynamic_cast<Properties *>(&resr);

    if (evt != nullptr)
    {
      // Pass as a reference to original modifyable collection without creating a copy
      return Log(severity, provId, body, *evt, trace_id, span_id, trace_flags, timestamp);
    }
#  endif
    Properties evtCopy = attributes;
    return Log(severity, provId, body, evtCopy, trace_id, span_id, trace_flags, timestamp);
  }

  void Log(opentelemetry::logs::Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const common::KeyValueIterable &attributes,
           opentelemetry::trace::TraceId trace_id,
           opentelemetry::trace::SpanId span_id,
           opentelemetry::trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept override
  {

#  ifdef OPENTELEMETRY_RTTI_ENABLED
    common::KeyValueIterable &attribs = const_cast<common::KeyValueIterable &>(attributes);
    Properties *evt                   = dynamic_cast<Properties *>(&attribs);
    // Properties *res                   = dynamic_cast<Properties *>(&resr);

    if (evt != nullptr)
    {
      // Pass as a reference to original modifyable collection without creating a copy
      return Log(severity, name, body, *evt, trace_id, span_id, trace_flags, timestamp);
    }
#  endif
    Properties evtCopy = attributes;
    return Log(severity, name, body, evtCopy, trace_id, span_id, trace_flags, timestamp);
  }

  virtual void Log(opentelemetry::logs::Severity severity,
                   nostd::string_view name,
                   nostd::string_view body,
                   Properties &evt,
                   opentelemetry::trace::TraceId trace_id,
                   opentelemetry::trace::SpanId span_id,
                   opentelemetry::trace::TraceFlags trace_flags,
                   common::SystemTimestamp timestamp) noexcept
  {
    // Populate Etw.EventName attribute at envelope level
    evt[ETW_FIELD_NAME] = ETW_VALUE_LOG;

#  ifdef HAVE_FIELD_TIME
    {
      auto timeNow        = std::chrono::system_clock::now().time_since_epoch();
      auto millis         = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow).count();
      evt[ETW_FIELD_TIME] = utils::formatUtcTimestampMsAsISO8601(millis);
    }
#  endif
    const auto &cfg = GetConfiguration(loggerProvider_);
    if (cfg.enableSpanId)
    {
      evt[ETW_FIELD_SPAN_ID] = ToLowerBase16(span_id);
    }
    if (cfg.enableTraceId)
    {
      evt[ETW_FIELD_TRACE_ID] = ToLowerBase16(trace_id);
    }
    // Populate ActivityId if enabled
    GUID ActivityId;
    LPGUID ActivityIdPtr = nullptr;
    if (cfg.enableActivityId)
    {
      if (CopySpanIdToActivityId(span_id, ActivityId))
      {
        ActivityIdPtr = &ActivityId;
      }
    }
    evt[ETW_FIELD_PAYLOAD_NAME]              = std::string(name.data(), name.size());
    std::chrono::system_clock::time_point ts = timestamp;
    int64_t tsMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(ts.time_since_epoch()).count();
    evt[ETW_FIELD_TIMESTAMP] = utils::formatUtcTimestampMsAsISO8601(tsMs);
    int severity_index       = static_cast<int>(severity);
    if (severity_index < 0 ||
        severity_index >= std::extent<decltype(opentelemetry::logs::SeverityNumToText)>::value)
    {
      std::stringstream sout;
      sout << "Invalid severity(" << severity_index << ")";
      evt[ETW_FIELD_LOG_SEVERITY_TEXT] = sout.str();
    }
    else
    {
      evt[ETW_FIELD_LOG_SEVERITY_TEXT] =
          opentelemetry::logs::SeverityNumToText[severity_index].data();
    }
    evt[ETW_FIELD_LOG_SEVERITY_NUM] = static_cast<uint32_t>(severity);
    evt[ETW_FIELD_LOG_BODY]         = std::string(body.data(), body.length());
    etwProvider().write(provHandle, evt, nullptr, nullptr, 0, encoding);
  }

  const nostd::string_view GetName() noexcept override { return std::string(); }
  // TODO : Flush and Shutdown method in main Logger API
  ~Logger() { etwProvider().close(provHandle); }
};

/**
 * @brief ETW LoggerProvider
 */
class LoggerProvider : public opentelemetry::logs::LoggerProvider
{
public:
  /**
   * @brief LoggerProvider options supplied during initialization.
   */
  TelemetryProviderConfiguration config_;

  /**
   * @brief Construct instance of LoggerProvider with given options
   * @param options Configuration options
   */
  LoggerProvider(TelemetryProviderOptions options) : opentelemetry::logs::LoggerProvider()
  {
    GetOption(options, "enableTraceId", config_.enableTraceId, true);
    GetOption(options, "enableSpanId", config_.enableSpanId, true);
    GetOption(options, "enableActivityId", config_.enableActivityId, false);

    // Determines what encoding to use for ETW events: TraceLogging Dynamic, MsgPack, XML, etc.
    config_.encoding = GetEncoding(options);
  }

  LoggerProvider() : opentelemetry::logs::LoggerProvider()
  {
    config_.encoding = ETWProvider::EventFormat::ETW_MANIFEST;
  }

  nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      nostd::string_view logger_name,
      nostd::string_view options,
      nostd::string_view library_name,
      nostd::string_view version    = "",
      nostd::string_view schema_url = "") override
  {
    UNREFERENCED_PARAMETER(options);
    UNREFERENCED_PARAMETER(library_name);
    UNREFERENCED_PARAMETER(version);
    UNREFERENCED_PARAMETER(schema_url);
    ETWProvider::EventFormat evtFmt = config_.encoding;
    return nostd::shared_ptr<opentelemetry::logs::Logger>{
        new (std::nothrow) etw::Logger(*this, logger_name, evtFmt)};
  }

  /**
   * @brief Obtain ETW Tracer.
   * @param name ProviderId (instrumentation name) - Name or GUID
   * @param args Additional arguments that controls `codec` of the provider.
   * Possible values are:
   * - "ETW"            - 'classic' Trace Logging Dynamic manifest ETW events.
   * - "MSGPACK"        - MessagePack-encoded binary payload ETW events.
   * - "XML"            - XML events (reserved for future use)
   * @param library_name Library name
   * @param version Library version
   * @param schema_url schema URL
   * @return
   */
  nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      nostd::string_view logger_name,
      nostd::span<nostd::string_view> args,
      nostd::string_view library_name,
      nostd::string_view version    = "",
      nostd::string_view schema_url = "") override
  {
    UNREFERENCED_PARAMETER(args);
    UNREFERENCED_PARAMETER(library_name);
    UNREFERENCED_PARAMETER(version);
    UNREFERENCED_PARAMETER(schema_url);
    ETWProvider::EventFormat evtFmt = config_.encoding;
    return nostd::shared_ptr<opentelemetry::logs::Logger>{
        new (std::nothrow) etw::Logger(*this, logger_name, evtFmt)};
  }
};

}  // namespace etw
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif  // ENABLE_LOGS_PREVIEW
