#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include <gtest/gtest.h>

using opentelemetry::common::KeyValueIterable;
using opentelemetry::logs::Logger;
using opentelemetry::logs::LoggerProvider;
using opentelemetry::logs::LogRecord;
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
  auto lp          = Provider::GetLoggerProvider();
  auto logger      = lp->GetLogger("TestLogger");
  string_view name = logger->getName();
  EXPECT_EQ("NOOP Logger", name);
}
/* TODO: add more tests */

// Define a basic Logger class
class TestLogger : public Logger
{
  // returns the name of the logger
  string_view getName() noexcept override { return "My custom implementation"; }

  // structured logging
  void log(const LogRecord &record) noexcept override {}
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

  auto lp          = Provider::GetLoggerProvider();
  auto logger      = lp->GetLogger("TestLogger");
  string_view name = logger->getName();
  EXPECT_EQ("My custom implementation", name);
}
