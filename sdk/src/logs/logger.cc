// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  include "opentelemetry/sdk/logs/logger.h"
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

  return nostd::unique_ptr<opentelemetry::logs::LogRecord>(
      context_->GetProcessor().MakeRecordable().release());
}

void Logger::EmitLogRecord(nostd::unique_ptr<opentelemetry::logs::LogRecord> &&log_record) noexcept
{
  if (!log_record)
  {
    return;
  }

  auto &processor = context_->GetProcessor();

  // TODO: Sampler (should include check for minSeverity)

  // Send the log recordable to the processor
  processor.OnEmit(std::unique_ptr<Recordable>(static_cast<Recordable *>(log_record.release())));
}

const opentelemetry::sdk::instrumentationscope::InstrumentationScope &
Logger::GetInstrumentationScope() const noexcept
{
  return *instrumentation_scope_;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
