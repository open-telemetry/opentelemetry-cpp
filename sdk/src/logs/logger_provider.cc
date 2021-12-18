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

nostd::shared_ptr<opentelemetry::logs::Logger> LoggerProvider::GetLogger(
    nostd::string_view logger_name,
    nostd::string_view options,
    nostd::string_view library_name,
    nostd::string_view library_version,
    nostd::string_view schema_url) noexcept
{
  // Ensure only one thread can read/write from the map of loggers
  std::lock_guard<std::mutex> lock_guard{mu_};

  // If a logger with a name "logger_name" already exists, return it
  for (auto &logger : loggers_)
  {
    auto& logger_lib = logger->GetInstrumentationLibrary();
    if (logger->GetName() == logger_name && logger_lib.equal(library_name, library_version, schema_url))
    {
      return nostd::shared_ptr<opentelemetry::logs::Logger>{logger};
    }
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

  auto lib = instrumentationlibrary::InstrumentationLibrary::Create(library_name, library_version,
                                                                    schema_url);
  loggers_.push_back(std::shared_ptr<opentelemetry::sdk::logs::Logger>(
      new Logger(logger_name, this->shared_from_this(), std::move(lib))));
  return nostd::shared_ptr<opentelemetry::logs::Logger>{loggers_.back()};
}

nostd::shared_ptr<opentelemetry::logs::Logger> LoggerProvider::GetLogger(
    nostd::string_view logger_name,
    nostd::span<nostd::string_view> args,
    nostd::string_view library_name,
    nostd::string_view library_version,
    nostd::string_view schema_url) noexcept
{
  return GetLogger(logger_name, "", library_name, library_version, schema_url);
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
