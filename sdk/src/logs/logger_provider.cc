// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/logger_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

namespace nostd    = opentelemetry::nostd;
namespace logs_api = opentelemetry::logs;

LoggerProvider::LoggerProvider() noexcept : processor_{nullptr} {}

nostd::shared_ptr<logs_api::Logger> LoggerProvider::GetLogger(nostd::string_view name,
                                                              nostd::string_view options) noexcept
{
  // Ensure only one thread can read/write from the map of loggers
  std::lock_guard<std::mutex> lock_guard{mu_};

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

  nostd::shared_ptr<logs_api::Logger> logger(new Logger(name, this->shared_from_this()));
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

std::shared_ptr<LogProcessor> LoggerProvider::GetProcessor() noexcept
{
  return processor_.load();
}

void LoggerProvider::SetProcessor(std::shared_ptr<LogProcessor> processor) noexcept
{
  processor_.store(processor);
}
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
