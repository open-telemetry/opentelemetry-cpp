#include <gtest/gtest.h>
#include <array>

#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::common::KeyValueIterable;
using opentelemetry::logs::Logger;
using opentelemetry::logs::LoggerProvider;
using opentelemetry::logs::LogRecord;
using opentelemetry::logs::Provider;
using opentelemetry::logs::Severity;
using opentelemetry::nostd::shared_ptr;
using opentelemetry::nostd::span;
using opentelemetry::nostd::string_view;

TEST(Logger, GetLoggerDefaultNoop)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");
  EXPECT_NE(nullptr, logger);
  EXPECT_EQ(logger->GetName(), "noop logger");
}

TEST(Logger, GetLoggerMethod)
{
  auto lp = Provider::GetLoggerProvider();

  // Get a logger with no arguments
  auto logger1 = lp->GetLogger("TestLogger1");

  // Get a logger with options passed
  auto logger2 = lp->GetLogger("TestLogger2", "Options");

  // Get a logger with arguments
  std::array<string_view, 1> sv{"string"};
  span<string_view> args{sv};
  auto logger3 = lp->GetLogger("TestLogger3", args);
}

TEST(Logger, LogMethod)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");

  // Test log(severity, name) method
  logger->Log(Severity::kError, "Error message");

  // Test log(LogRecord)
  auto r      = opentelemetry::nostd::shared_ptr<LogRecord>(new LogRecord);
  r->name     = "Log Record";
  r->severity = Severity::kInfo;
  logger->Log(r);
}

// Define a basic Logger class
class TestLogger : public Logger
{
  void Log(opentelemetry::nostd::shared_ptr<LogRecord> record) noexcept override {}
  opentelemetry::nostd::string_view GetName() noexcept override { return "test logger"; };
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
  // Push the new loggerprovider class into the API
  auto test_provider = shared_ptr<LoggerProvider>(new TestProvider());
  Provider::SetLoggerProvider(test_provider);

  auto lp = Provider::GetLoggerProvider();

  // Get a logger instance and check whether it's GetName() method returns
  // "test logger" as defined in the custom implementation
  auto logger = lp->GetLogger("TestLogger");
  ASSERT_EQ("test logger", logger->GetName());
}
