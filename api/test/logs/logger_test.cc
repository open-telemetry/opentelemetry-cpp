#include <gtest/gtest.h>
#include <array>

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::logs::Logger;
using opentelemetry::logs::LoggerProvider;
using opentelemetry::logs::Provider;
using opentelemetry::logs::Severity;
using opentelemetry::nostd::shared_ptr;
using opentelemetry::nostd::span;
using opentelemetry::nostd::string_view;

TEST(Logger, GetLoggerDefault)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");
  EXPECT_NE(nullptr, logger);
}

TEST(Logger, GetNoopLoggerName)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");
}

TEST(Logger, GetNoopLoggerNameWithArgs)
{
  auto lp = Provider::GetLoggerProvider();

  std::array<string_view, 1> sv{"string"};
  span<string_view> args{sv};
  auto logger = lp->GetLogger("NoopLoggerWithArgs", args);
  // should probably also test that arguments were set properly too
  // by adding a getArgs() method in NoopLogger
}

TEST(Logger, NoopLog)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");
  logger->Log("Noop log name");
}

// Define a basic Logger class
class TestLogger : public Logger
{
  void Log(Severity severity,
           string_view name,
           string_view body,
           const opentelemetry::common::KeyValueIterable &resource,
           const opentelemetry::common::KeyValueIterable &attributes,
           opentelemetry::trace::TraceId trace_id,
           opentelemetry::trace::SpanId span_id,
           opentelemetry::trace::TraceFlags trace_flags,
           opentelemetry::core::SystemTimestamp timestamp) noexcept override
  {}
};

// Define a basic LoggerProvider class that returns an instance of the logger class defined above
class TestProvider : public LoggerProvider
{
  shared_ptr<Logger> GetLogger(string_view library_name, string_view options = "") override
  {
    return shared_ptr<Logger>(new TestLogger());
  }

  shared_ptr<Logger> GetLogger(string_view library_name, span<string_view> args) override
  {
    return shared_ptr<Logger>(new TestLogger());
  }
};

TEST(Logger, PushLoggerImplementation)
{
  // Push the new loggerprovider class into the global singleton
  auto test_provider = shared_ptr<LoggerProvider>(new TestProvider());
  Provider::SetLoggerProvider(test_provider);

  auto lp = Provider::GetLoggerProvider();

  // GetLogger(name, options) function
  auto logger = lp->GetLogger("TestLogger");

  // GetLogger(name, args) function
  std::array<string_view, 1> sv{"string"};
  span<string_view> args{sv};
  auto logger2 = lp->GetLogger("TestLogger2", args);
}
