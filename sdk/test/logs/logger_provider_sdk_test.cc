// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <array>
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/logs/log_record.h"
#include "opentelemetry/sdk/logs/logger.h"
#include "opentelemetry/sdk/logs/logger_provider.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;

TEST(LoggerProviderSDK, PushToAPI)
{
  auto lp = opentelemetry::nostd::shared_ptr<opentelemetry::logs::LoggerProvider>(
      new opentelemetry::sdk::logs::LoggerProvider());
  opentelemetry::logs::Provider::SetLoggerProvider(lp);

  // Check that the loggerprovider was correctly pushed into the API
  ASSERT_EQ(lp, opentelemetry::logs::Provider::GetLoggerProvider());
}

TEST(LoggerProviderSDK, LoggerProviderGetLoggerSimple)
{
  auto lp = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());

  auto logger1 = lp->GetLogger("logger1");
  auto logger2 = lp->GetLogger("logger2");

  // Check that the logger is not nullptr
  ASSERT_NE(logger1, nullptr);
  ASSERT_NE(logger2, nullptr);

  // Check that two loggers with different names aren't the same instance
  ASSERT_NE(logger1, logger2);

  // Check that two loggers with the same name are the same instance
  auto logger3 = lp->GetLogger("logger1");
  ASSERT_EQ(logger1, logger3);
}

TEST(LoggerProviderSDK, LoggerProviderLoggerArguments)
{
  // Currently, arguments are not supported by the loggers.
  // TODO: Once the logging spec defines what arguments are allowed, add more
  // detail to this test
  auto lp = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());

  auto logger1 = lp->GetLogger("logger1", "");

  // Check GetLogger(logger_name, args)
  std::array<opentelemetry::nostd::string_view, 1> sv{"string"};
  opentelemetry::nostd::span<opentelemetry::nostd::string_view> args{sv};
  auto logger2 = lp->GetLogger("logger2", args);
}

class DummyProcessor : public LogProcessor
{
  std::unique_ptr<Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<Recordable>(new LogRecord);
  }

  void OnReceive(std::unique_ptr<Recordable> &&record) noexcept {}
  bool ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
};

TEST(LoggerProviderSDK, GetAndSetProcessor)
{
  // Create a LoggerProvider without a processor
  LoggerProvider lp;
  ASSERT_EQ(lp.GetProcessor(), nullptr);

  // Create a new processor and check if it is pushed correctly
  std::shared_ptr<LogProcessor> proc2 = std::shared_ptr<LogProcessor>(new DummyProcessor());
  lp.SetProcessor(proc2);
  ASSERT_EQ(proc2, lp.GetProcessor());
}
