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

#include "opentelemetry/sdk/logs/log_record.h"
#include "opentelemetry/sdk/logs/logger.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;

TEST(LoggerSDK, LogToNullProcessor)
{
  // Confirm Logger::Log() does not have undefined behavior
  // even when there is no processor set
  // since it calls Processor::OnReceive()

  auto lp     = std::shared_ptr<opentelemetry::logs::LoggerProvider>(new LoggerProvider());
  auto logger = lp->GetLogger("logger");

  // Log a sample log record to a nullptr processor
  logger->Debug("Test log");
}

class MockProcessor final : public LogProcessor
{
private:
  std::shared_ptr<LogRecord> record_received_;

public:
  // A processor used for testing that keeps a track of the recordable it received
  explicit MockProcessor(std::shared_ptr<LogRecord> record_received) noexcept
      : record_received_(record_received)
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<Recordable>(new LogRecord);
  }
  // OnReceive stores the record it receives into the shared_ptr recordable passed into its
  // constructor
  void OnReceive(std::unique_ptr<Recordable> &&record) noexcept
  {
    // Cast the recordable received into a concrete LogRecord type
    auto copy = std::shared_ptr<LogRecord>(static_cast<LogRecord *>(record.release()));

    // Copy over the received log record's severity, name, and body fields over to the recordable
    // passed in the constructor
    record_received_->SetSeverity(copy->GetSeverity());
    record_received_->SetName(copy->GetName());
    record_received_->SetBody(copy->GetBody());
  }
  bool ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
  }
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept
  {
    return true;
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
  auto shared_recordable = std::shared_ptr<LogRecord>(new LogRecord());
  auto processor         = std::shared_ptr<LogProcessor>(new MockProcessor(shared_recordable));
  lp->SetProcessor(processor);
  ASSERT_EQ(processor, lp->GetProcessor());

  // Check that the recordable created by the Log() statement is set properly
  logger->Log(opentelemetry::logs::Severity::kWarn, "Log Name", "Log Message");

  ASSERT_EQ(shared_recordable->GetSeverity(), opentelemetry::logs::Severity::kWarn);
  ASSERT_EQ(shared_recordable->GetName(), "Log Name");
  ASSERT_EQ(shared_recordable->GetBody(), "Log Message");
}
