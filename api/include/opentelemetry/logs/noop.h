#pragma once
// Please refer to provider.h for documentation on how to obtain a Logger object.
//
// This file is part of the internal implementation of OpenTelemetry. Nothing in this file should be
// used directly. Please refer to logger.h for documentation on these interfaces.

#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{
/**
 * No-op implementation of Logger. This class should not be used directly. It should only be
 * instantiated using a LoggerProvider's GetLogger() call.
 */
class NoopLogger final : public Logger
{
public:
  NoopLogger() = default;

  // returns the name of the logger
  nostd::string_view getName() noexcept override { return "NOOP Logger"; }

  void log(const LogRecord &record) noexcept override {}
};

/**
 * No-op implementation of a LoggerProvider.
 */
class NoopLoggerProvider final : public opentelemetry::logs::LoggerProvider
{
public:
  NoopLoggerProvider()
      : logger_{
            nostd::shared_ptr<opentelemetry::logs::NoopLogger>(new opentelemetry::logs::NoopLogger)}
  {}

  nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                      nostd::string_view options) override
  {
    return logger_;
  }

  nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                      nostd::span<nostd::string_view> args) override
  {
    return logger_;
  }

private:
  nostd::shared_ptr<opentelemetry::logs::Logger> logger_;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
