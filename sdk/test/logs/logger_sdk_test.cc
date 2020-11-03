/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "opentelemetry/sdk/logs/logger.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;

TEST(LoggerSDK, LogToNullProcessor)
{
  // Confirm Logger::log() does not have undefined behavior
  // even when there is no processor set
  // since it calls Processor::OnReceive()

  auto lp     = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());
  auto logger = lp->GetLogger("logger");

  // Log a sample log record to a nullptr processor
  opentelemetry::logs::LogRecord r;
  r.name = "Test log";
  logger->log(r);
}

class DummyProcessor : public LogProcessor
{
  void OnReceive(std::unique_ptr<opentelemetry::logs::LogRecord> &&record) noexcept {}
  ShutdownResult ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return ShutdownResult::kSuccess;
  }
  ShutdownResult Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return ShutdownResult::kSuccess;
  }
};

TEST(LoggerSDK, LogToAProcessor)
{
  // Create an API LoggerProvider and logger
  auto api_lp = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());
  auto logger = api_lp->GetLogger("logger");

  // Cast the API LoggerProvider to an SDK Logger Provider and assert that it is still the same
  // LoggerProvider by checking that getting a logger with the same name as the previously defined
  // logger is the same instance
  auto lp      = static_cast<LoggerProvider *>(api_lp.get());
  auto logger2 = lp->GetLogger("logger");
  ASSERT_EQ(logger, logger2);

  // Set a processor for the LoggerProvider
  std::shared_ptr<LogProcessor> processor = std::shared_ptr<LogProcessor>(new DummyProcessor());
  lp->SetProcessor(processor);
  ASSERT_EQ(processor, lp->GetProcessor());

  // Should later introduce a way to assert that
  // the logger's processor is the same as "proc"
  // and that the logger's processor is the same as lp's processor

  // Log a sample log record to the processor
  opentelemetry::logs::LogRecord r;
  r.name = "Test log";
  logger->log(r);
}
