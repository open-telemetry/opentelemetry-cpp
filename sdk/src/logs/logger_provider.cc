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

LoggerProvider::~LoggerProvider()
{
  // Logger hold the shared pointer to the context. So we can not use destructor of LoggerContext to
  // Shutdown and flush all pending recordables when we hasve more than one loggers.These
  // recordables may use the raw pointer of instrumentation_library_ in Logger
  if (context_)
  {
    context_->Shutdown();
  }
}

nostd::shared_ptr<opentelemetry::logs::Logger> LoggerProvider::GetLogger(
    nostd::string_view logger_name,
    nostd::string_view options,
    nostd::string_view library_name,
    nostd::string_view library_version,
    nostd::string_view schema_url) noexcept
{
  // Ensure only one thread can read/write from the map of loggers
  std::lock_guard<std::mutex> lock_guard{lock_};

  // If a logger with a name "logger_name" already exists, return it
  for (auto &logger : loggers_)
  {
    auto &logger_lib = logger->GetInstrumentationLibrary();
    if (logger->GetName() == logger_name &&
        logger_lib.equal(library_name, library_version, schema_url))
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

  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/logs/data-model.md#field-instrumentationscope
  opentelemetry::nostd::unique_ptr<instrumentationlibrary::InstrumentationLibrary> lib;
  if (library_name.empty())
  {
    lib = instrumentationlibrary::InstrumentationLibrary::Create(logger_name, library_version,
                                                                 schema_url);
  }
  else
  {
    lib = instrumentationlibrary::InstrumentationLibrary::Create(library_name, library_version,
                                                                 schema_url);
  }

  loggers_.push_back(std::shared_ptr<opentelemetry::sdk::logs::Logger>(
      new Logger(logger_name, context_, std::move(lib))));
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

void LoggerProvider::AddProcessor(std::unique_ptr<LogProcessor> processor) noexcept
{
  context_->AddProcessor(std::move(processor));
}

const opentelemetry::sdk::resource::Resource &LoggerProvider::GetResource() const noexcept
{
  return context_->GetResource();
}

bool LoggerProvider::Shutdown() noexcept
{
  return context_->Shutdown();
}

bool LoggerProvider::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return context_->ForceFlush(timeout);
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
