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
namespace nostd  = opentelemetry::nostd;
namespace trace  = opentelemetry::trace;

// Check that the default logger is a noop logger instance
TEST(Logger, GetLoggerDefault)
{
  auto lp = Provider::GetLoggerProvider();
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp->GetLogger("TestLogger", "", "opentelelemtry_library", "", schema_url);
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
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  lp->GetLogger("NoopLoggerWithArgs", args, "opentelelemtry_library", "", schema_url);

  // GetLogger(name, string options)
  lp->GetLogger("NoopLoggerWithOptions", "options", "opentelelemtry_library", "", schema_url);
}

// Test the Log() overloads
TEST(Logger, LogMethodOverloads)
{
  auto lp = Provider::GetLoggerProvider();
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp->GetLogger("TestLogger", "", "opentelelemtry_library", "", schema_url);

  // Create a map to test the logs with
  std::map<std::string, std::string> m = {{"key1", "value1"}};

  // Log overloads
  logger->Log(Severity::kTrace, "Test log message");
  logger->Log(Severity::kInfo, "Test log message");
  logger->Log(Severity::kDebug, m);
  logger->Log(Severity::kWarn, "Logging a map", m);
  logger->Log(Severity::kError, {{"key1", "value 1"}, {"key2", 2}});
  logger->Log(Severity::kFatal, "Logging an initializer list", {{"key1", "value 1"}, {"key2", 2}});

  // Deprecated Log overloads
  logger->Log(Severity::kTrace, "Log name", "Test log message");
  logger->Log(Severity::kWarn, "Log name", "Logging a map", m, {}, {}, {},
              std::chrono::system_clock::now());
  logger->Log(Severity::kError, "Log name", "Logging a map", {{"key1", "value 1"}, {"key2", 2}}, {},
              {}, {}, std::chrono::system_clock::now());
  logger->Trace("Log name", "Test log message");
  logger->Debug("Log name", "Test log message");
  logger->Info("Log name", "Test log message");
  logger->Warn("Log name", "Test log message");
  logger->Error("Log name", "Test log message");
  logger->Fatal("Log name", "Test log message");

  // Severity methods
  logger->Trace("Test log message");
  logger->Trace("Test log message", m);
  logger->Trace("Test log message", {{"key1", "value 1"}, {"key2", 2}});
  logger->Trace(m);
  logger->Trace({{"key1", "value 1"}, {"key2", 2}});
  logger->Debug("Test log message");
  logger->Debug("Test log message", m);
  logger->Debug("Test log message", {{"key1", "value 1"}, {"key2", 2}});
  logger->Debug(m);
  logger->Debug({{"key1", "value 1"}, {"key2", 2}});
  logger->Info("Test log message");
  logger->Info("Test log message", m);
  logger->Info("Test log message", {{"key1", "value 1"}, {"key2", 2}});
  logger->Info(m);
  logger->Info({{"key1", "value 1"}, {"key2", 2}});
  logger->Warn("Test log message");
  logger->Warn("Test log message", m);
  logger->Warn("Test log message", {{"key1", "value 1"}, {"key2", 2}});
  logger->Warn(m);
  logger->Warn({{"key1", "value 1"}, {"key2", 2}});
  logger->Error("Test log message");
  logger->Error("Test log message", m);
  logger->Error("Test log message", {{"key1", "value 1"}, {"key2", 2}});
  logger->Error(m);
  logger->Error({{"key1", "value 1"}, {"key2", 2}});
  logger->Fatal("Test log message");
  logger->Fatal("Test log message", m);
  logger->Fatal("Test log message", {{"key1", "value 1"}, {"key2", 2}});
  logger->Fatal(m);
  logger->Fatal({{"key1", "value 1"}, {"key2", 2}});
}

// Define a basic Logger class
class TestLogger : public Logger
{
  const nostd::string_view GetName() noexcept override { return "test logger"; }

  void Log(Severity severity,
           string_view body,
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
  nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                      nostd::string_view options,
                                      nostd::string_view library_name,
                                      nostd::string_view library_version = "",
                                      nostd::string_view schema_url      = "") override
  {
    return shared_ptr<Logger>(new TestLogger());
  }

  nostd::shared_ptr<Logger> GetLogger(nostd::string_view logger_name,
                                      nostd::span<nostd::string_view> args,
                                      nostd::string_view library_name,
                                      nostd::string_view library_version = "",
                                      nostd::string_view schema_url      = "") override
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
  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp->GetLogger("TestLogger", "", "opentelelemtry_library", "", schema_url);
  ASSERT_EQ("test logger", logger->GetName());
}
#endif
