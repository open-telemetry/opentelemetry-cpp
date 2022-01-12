// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/logger_provider.h"

#  include <memory>
#  include <mutex>
#  include <string>
#  include <unordered_map>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

namespace nostd    = opentelemetry::nostd;
namespace logs_api = opentelemetry::logs;

LoggerProvider::LoggerProvider(std::unique_ptr<LogProcessor> &&processor,
                               opentelemetry::sdk::resource::Resource resource) noexcept
{
  std::vector<std::unique_ptr<LogProcessor>> processors;
  processors.emplace_back(std::move(processor));
  context_ = std::make_shared<sdk::logs::LoggerContext>(std::move(processors), std::move(resource));
}

LoggerProvider::LoggerProvider(std::vector<std::unique_ptr<LogProcessor>> &&processors,
                               opentelemetry::sdk::resource::Resource resource) noexcept
    : context_{
          std::make_shared<sdk::logs::LoggerContext>(std::move(processors), std::move(resource))}
{}

LoggerProvider::LoggerProvider() noexcept
    : context_{
          std::make_shared<sdk::logs::LoggerContext>(std::vector<std::unique_ptr<LogProcessor>>{})}
{}

LoggerProvider::LoggerProvider(std::shared_ptr<sdk::logs::LoggerContext> context) noexcept
    : context_{context}
{}

nostd::shared_ptr<logs_api::Logger> LoggerProvider::GetLogger(nostd::string_view name,
                                                              nostd::string_view options) noexcept
{
  // Ensure only one thread can read/write from the map of loggers
  std::lock_guard<std::mutex> lock_guard{lock_};

  // If a logger with a name "name" already exists, return it
  auto loggerkv = loggers_.find(name.data());
  if (loggerkv != loggers_.end())
  {
    return nostd::shared_ptr<logs_api::Logger>(loggerkv->second);
  }

  // Check if creating a new logger would exceed the max number of loggers
  // TODO: Remove the noexcept from the API's and SDK's GetLogger(~)
  /*
  if (loggers_.size() > MAX_LOGGER_COUNT)
  {
#if __EXCEPTIONS
    throw std::length_error("Number of loggers exceeds max count");
#else
    std::terminate();
#endif
  }
  */

  // If no logger with that name exists yet, create it and add it to the map of loggers

  nostd::shared_ptr<logs_api::Logger> logger(new Logger(name, context_));
  loggers_[name.data()] = logger;
  return logger;
}

nostd::shared_ptr<logs_api::Logger> LoggerProvider::GetLogger(
    nostd::string_view name,
    nostd::span<nostd::string_view> args) noexcept
{
  // Currently, no args support
  return GetLogger(name);
}

void LoggerProvider::AddProcessor(std::unique_ptr<LogProcessor> processor) noexcept
{
  context_->AddProcessor(std::move(processor));
}

const opentelemetry::sdk::resource::Resource &LoggerProvider::GetResource() const noexcept
{
  return context_->GetResource();
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
