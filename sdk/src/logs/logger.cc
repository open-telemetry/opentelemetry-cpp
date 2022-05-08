// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  include "opentelemetry/sdk/logs/logger.h"
#  include "opentelemetry/sdk/logs/log_record.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/trace/provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace common    = opentelemetry::common;

Logger::Logger(nostd::string_view name,
               std::shared_ptr<LoggerContext> context,
               std::unique_ptr<instrumentationlibrary::InstrumentationLibrary>
                   instrumentation_library) noexcept
    : logger_name_(std::string(name)),
      instrumentation_library_(std::move(instrumentation_library)),
      context_(context)
{}

const nostd::string_view Logger::GetName() noexcept
{
  return logger_name_;
}

/**
 * Create and populate recordable with the log event's fields passed in.
 * The timestamp, severity, traceid, spanid, and traceflags, are injected
 * if the user does not specify them.
 */
void Logger::Log(opentelemetry::logs::Severity severity,
                 nostd::string_view body,
                 const common::KeyValueIterable &attributes,
                 trace_api::TraceId trace_id,
                 trace_api::SpanId span_id,
                 trace_api::TraceFlags trace_flags,
                 common::SystemTimestamp timestamp) noexcept
{
  // If this logger does not have a processor, no need to create a log record
  if (!context_)
  {
    return;
  }
  auto &processor = context_->GetProcessor();

  // TODO: Sampler (should include check for minSeverity)

  auto recordable = processor.MakeRecordable();
  if (recordable == nullptr)
  {
    OTEL_INTERNAL_LOG_ERROR("[LOGGER] Recordable creation failed");
    return;
  }

  // Populate recordable fields
  recordable->SetTimestamp(timestamp);
  recordable->SetSeverity(severity);
  recordable->SetBody(body);
  recordable->SetInstrumentationLibrary(GetInstrumentationLibrary());

  recordable->SetResource(context_->GetResource());

  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    recordable->SetAttribute(key, value);
    return true;
  });

  // Inject trace_id/span_id/trace_flags if none is set by user
  auto provider     = trace_api::Provider::GetTracerProvider();
  auto tracer       = provider->GetTracer(logger_name_);
  auto span_context = tracer->GetCurrentSpan()->GetContext();

  // Leave these fields in the recordable empty if neither the passed in values
  // nor the context values are valid (e.g. the application is not using traces)

  // TraceId
  if (trace_id.IsValid())
  {
    recordable->SetTraceId(trace_id);
  }
  else if (span_context.trace_id().IsValid())
  {
    recordable->SetTraceId(span_context.trace_id());
  }

  // SpanId
  if (span_id.IsValid())
  {
    recordable->SetSpanId(span_id);
  }
  else if (span_context.span_id().IsValid())
  {
    recordable->SetSpanId(span_context.span_id());
  }

  // TraceFlags
  if (trace_flags.IsSampled())
  {
    recordable->SetTraceFlags(trace_flags);
  }
  else if (span_context.trace_flags().IsSampled())
  {
    recordable->SetTraceFlags(span_context.trace_flags());
  }

  // Send the log record to the processor
  processor.OnReceive(std::move(recordable));
}

const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary &
Logger::GetInstrumentationLibrary() const noexcept
{
  return *instrumentation_library_;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
