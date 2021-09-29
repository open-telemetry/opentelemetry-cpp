// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <gtest/gtest.h>
#  include <array>

#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/logs/logger.h"
#  include "opentelemetry/logs/provider.h"
#  include "opentelemetry/nostd/shared_ptr.h"

using opentelemetry::logs::Logger;
using opentelemetry::logs::LoggerProvider;
using opentelemetry::logs::Provider;
using opentelemetry::logs::Severity;
using opentelemetry::nostd::shared_ptr;
using opentelemetry::nostd::span;
using opentelemetry::nostd::string_view;
namespace common = opentelemetry::common;
namespace nostd = opentelemetry::nostd;
namespace trace = opentelemetry::trace;


// Check that the default logger is a noop logger instance
TEST(Logger, GetLoggerDefault)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");
  auto name   = logger->GetName();
  EXPECT_NE(nullptr, logger);
  EXPECT_EQ(name, "noop logger");
}

// Test the two additional overloads for GetLogger()
TEST(Logger, GetNoopLoggerNameWithArgs)
{
  auto lp = Provider::GetLoggerProvider();

  // GetLogger(name, list(args))
  std::array<string_view, 1> sv{"string"};
  span<string_view> args{sv};
  lp->GetLogger("NoopLoggerWithArgs", args);

  // GetLogger(name, string options)
  lp->GetLogger("NoopLoggerWithOptions", "options");
}

// Test the Log() overloads
TEST(Logger, LogMethodOverloads)
{
  auto lp     = Provider::GetLoggerProvider();
  auto logger = lp->GetLogger("TestLogger");

  // Create a map to test the logs with
  std::map<std::string, std::string> m = {{"key1", "value1"}};

  // Log overloads
  logger->Log(Severity::kTrace, "Test log message");
  logger->Log(Severity::kInfo, "Logging a message", "Test log message");
  logger->Log(Severity::kDebug, m);
  logger->Log(Severity::kWarn, "Logging a map", m);
  logger->Log(Severity::kError, {{"key1", "value 1"}, {"key2", 2}});
  logger->Log(Severity::kFatal, "Logging an initializer list", {{"key1", "value 1"}, {"key2", 2}});

  // Severity methods
  logger->Trace("Test log message");
  logger->Debug("Logging a message", "Test log message");
  logger->Info(m);
  logger->Warn("Logging a map", m);
  logger->Error({{"key1", "value 1"}, {"key2", 2}});
  logger->Fatal("Logging an initializer list", {{"key1", "value 1"}, {"key2", 2}});
}

// Define a basic Logger class
class TestLogger : public Logger
{
  const nostd::string_view GetName() noexcept override { return "test logger"; }

  void Log(Severity severity,
           string_view name,
           string_view body,
           const common::KeyValueIterable &resource,
           const common::KeyValueIterable &attributes,
           trace::TraceId trace_id,
           trace::SpanId span_id,
           trace::TraceFlags trace_flags,
           common::SystemTimestamp timestamp) noexcept override
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

  // Check that the implementation was pushed by calling TestLogger's GetName()
  auto logger = lp->GetLogger("TestLogger");
  ASSERT_EQ("test logger", logger->GetName());
}
#endif
