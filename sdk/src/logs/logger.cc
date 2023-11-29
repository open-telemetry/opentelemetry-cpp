// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/logger.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk_config.h"
#include "opentelemetry/trace/provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
namespace trace_api = opentelemetry::trace;
namespace nostd     = opentelemetry::nostd;
namespace common    = opentelemetry::common;

Logger::Logger(
    nostd::string_view name,
    std::shared_ptr<LoggerContext> context,
    std::unique_ptr<instrumentationscope::InstrumentationScope> instrumentation_scope) noexcept
    : logger_name_(std::string(name)),
      instrumentation_scope_(std::move(instrumentation_scope)),
      context_(context)
{}

const nostd::string_view Logger::GetName() noexcept
{
  return logger_name_;
}

nostd::unique_ptr<opentelemetry::logs::LogRecord> Logger::CreateLogRecord() noexcept
{
  // If this logger does not have a processor, no need to create a log recordable
  if (!context_)
  {
    return nullptr;
  }

  auto recordable = context_->GetProcessor().MakeRecordable();

  recordable->SetObservedTimestamp(std::chrono::system_clock::now());

  if (opentelemetry::context::RuntimeContext::GetCurrent().HasKey(opentelemetry::trace::kSpanKey))
  {
    opentelemetry::context::ContextValue context_value =
        opentelemetry::context::RuntimeContext::GetCurrent().GetValue(
            opentelemetry::trace::kSpanKey);
    if (nostd::holds_alternative<nostd::shared_ptr<opentelemetry::trace::Span>>(context_value))
    {
      nostd::shared_ptr<opentelemetry::trace::Span> &data =
          nostd::get<nostd::shared_ptr<opentelemetry::trace::Span>>(context_value);
      if (data)
      {
        recordable->SetTraceId(data->GetContext().trace_id());
        recordable->SetTraceFlags(data->GetContext().trace_flags());
        recordable->SetSpanId(data->GetContext().span_id());
      }
    }
    else if (nostd::holds_alternative<nostd::shared_ptr<trace::SpanContext>>(context_value))
    {
      nostd::shared_ptr<trace::SpanContext> &data =
          nostd::get<nostd::shared_ptr<trace::SpanContext>>(context_value);
      if (data)
      {
        recordable->SetTraceId(data->trace_id());
        recordable->SetTraceFlags(data->trace_flags());
        recordable->SetSpanId(data->span_id());
      }
    }
  }

  return nostd::unique_ptr<opentelemetry::logs::LogRecord>(recordable.release());
}

void Logger::EmitLogRecord(nostd::unique_ptr<opentelemetry::logs::LogRecord> &&log_record) noexcept
{
  if (!log_record)
  {
    return;
  }

  std::unique_ptr<Recordable> recordable =
      std::unique_ptr<Recordable>(static_cast<Recordable *>(log_record.release()));
  recordable->SetResource(context_->GetResource());
  recordable->SetInstrumentationScope(GetInstrumentationScope());

  auto &processor = context_->GetProcessor();

  // TODO: Sampler (should include check for minSeverity)

  // Send the log recordable to the processor
  processor.OnEmit(std::move(recordable));
}

const opentelemetry::sdk::instrumentationscope::InstrumentationScope &
Logger::GetInstrumentationScope() const noexcept
{
  return *instrumentation_scope_;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
