/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "opentelemetry/sdk/logs/logger_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

LoggerProvider::LoggerProvider() noexcept : processor_{nullptr} {}

opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> LoggerProvider::GetLogger(
    opentelemetry::nostd::string_view name,
    opentelemetry::nostd::string_view options) noexcept
{
  // Ensure only one thread can read/write from the map of loggers
  std::lock_guard<std::mutex> lock_guard{mu_};

  // If a logger with a name "name" already exists, return it
  auto loggerkv = loggers_.find(name.data());
  if (loggerkv != loggers_.end())
  {
    return opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger>(loggerkv->second);
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

  opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger(
      new Logger(name, this->shared_from_this()));
  loggers_[name.data()] = logger;
  return logger;
}

opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> LoggerProvider::GetLogger(
    opentelemetry::nostd::string_view name,
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
