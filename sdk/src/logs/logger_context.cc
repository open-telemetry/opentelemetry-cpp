// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/logger_context.h"
#  include "opentelemetry/sdk/logs/multi_log_processor.h"

#  include <memory>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

LoggerContext::LoggerContext(std::vector<std::unique_ptr<LogProcessor>> &&processors,
                             opentelemetry::sdk::resource::Resource resource) noexcept
    : resource_(resource),
      processor_(std::unique_ptr<LogProcessor>(new MultiLogProcessor(std::move(processors))))
{}

void LoggerContext::AddProcessor(std::unique_ptr<LogProcessor> processor) noexcept
{
  auto multi_processor = static_cast<MultiLogProcessor *>(processor_.get());
  multi_processor->AddProcessor(std::move(processor));
}

LogProcessor &LoggerContext::GetProcessor() const noexcept
{
  return *processor_;
}

const opentelemetry::sdk::resource::Resource &LoggerContext::GetResource() const noexcept
{
  return resource_;
}

bool LoggerContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return processor_->ForceFlush(timeout);
}

bool LoggerContext::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return processor_->ForceFlush(timeout);
}

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
