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

TEST(LoggerTest, GetLoggerDefaultNoop)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");
  EXPECT_NE(nullptr, logger);
  EXPECT_EQ(logger->GetName(), "noop logger");
}

TEST(LoggerTest, GetLogger)
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

TEST(LoggerTest, PushLoggerImplementation)
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

TEST(Logger, LogMethodOverloads)
{
  // Use the same TestProvider and TestLogger from the previous test
  auto test_provider = shared_ptr<LoggerProvider>(new TestProvider());
  Provider::SetLoggerProvider(test_provider);

  auto lp = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");

  // Check that calling the Log() overloads correctly constructs a log record which is automatically put into the static logger_ for testing
  
  // Test Log(severity, name, message) method
  logger->Log(Severity::kError, "Log Name", "This is the log message");
  ASSERT_EQ(record_->severity, Severity::kError);
  ASSERT_EQ(record_->name, "Log Name");
  ASSERT_EQ(record_->body, "This is the log message");

  // Test Trace(name, KVIterable) method
  std::map<std::string, std::string> m = {{"key1", "val1"}, {"key2", "val2"}};
  logger->Trace("Logging a map", m);
  ASSERT_EQ(record_->severity, Severity::kTrace);
  ASSERT_EQ(record_->name, "Logging a map");
  ASSERT_EQ(record_->attributes->size(), 2);
}

TEST(LogRecord, SetDefault)
{
  LogRecord r;

  // Check that the timestamp is set to 0 by default
  ASSERT_EQ(r.timestamp, opentelemetry::core::SystemTimestamp(std::chrono::seconds(0)));

  // Check that the severity is set to kDefault by default
  ASSERT_EQ(r.severity, Severity::kDefault);

  // Check that trace_id is set to all zeros by default
  char trace_buf[32];
  r.trace_id.ToLowerBase16(trace_buf);
  ASSERT_EQ(std::string(trace_buf, sizeof(trace_buf)), "00000000000000000000000000000000");

  // Check that span_id is set to all zeros by default
  char span_buf[16];
  r.span_id.ToLowerBase16(span_buf);
  ASSERT_EQ(std::string(span_buf, sizeof(span_buf)), "0000000000000000");

  // Check that trace_flags is set to all zeros by default
  char flags_buf[2];
  r.trace_flags.ToLowerBase16(flags_buf);
  ASSERT_EQ(std::string(flags_buf, sizeof(flags_buf)), "00");
}
