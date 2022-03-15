// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/logs/logger.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/logs/logger_context.h"
#  include "opentelemetry/sdk/logs/logger_provider.h"

#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
class LoggerProvider;

class Logger final : public opentelemetry::logs::Logger
{
public:
  /**
   * Initialize a new logger.
   * @param name The name of this logger instance
   * @param context The logger provider that owns this logger.
   */
  explicit Logger(
      opentelemetry::nostd::string_view name,
      std::shared_ptr<LoggerContext> context,
      std::unique_ptr<instrumentationlibrary::InstrumentationLibrary> instrumentation_library =
          instrumentationlibrary::InstrumentationLibrary::Create("")) noexcept;

  /**
   * Returns the name of this logger.
   */
  const opentelemetry::nostd::string_view GetName() noexcept override;

  /**
   * Writes a log record into the processor.
   * @param severity the severity level of the log event.
   * @param name the name of the log event.
   * @param message the string message of the log (perhaps support std::fmt or fmt-lib format).
   * with the log event.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with the log event.
   * @param trace_id the trace id associated with the log event.
   * @param span_id the span id associate with the log event.
   * @param trace_flags the trace flags associated with the log event.
   * @param timestamp the timestamp the log record was created.
   * @throws No exceptions under any circumstances.   */
  void Log(opentelemetry::logs::Severity severity,
           nostd::string_view name,
           nostd::string_view body,
           const opentelemetry::common::KeyValueIterable &attributes,
           opentelemetry::trace::TraceId trace_id,
           opentelemetry::trace::SpanId span_id,
           opentelemetry::trace::TraceFlags trace_flags,
           opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /** Returns the associated instruementation library */
  const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary &
  GetInstrumentationLibrary() const noexcept;

private:
  // The name of this logger
  std::string logger_name_;

  // order of declaration is important here - instrumentation library should destroy after
  // logger-context.
  std::unique_ptr<instrumentationlibrary::InstrumentationLibrary> instrumentation_library_;
  std::shared_ptr<LoggerContext> context_;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
