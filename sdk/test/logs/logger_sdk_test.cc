// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/log_record.h"
#  include "opentelemetry/sdk/logs/logger.h"

#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;

TEST(LoggerSDK, LogToNullProcessor)
{
  // Confirm Logger::Log() does not have undefined behavior
  // even when there is no processor set
  // since it calls Processor::OnReceive()

  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp->GetLogger("logger", "", "opentelelemtry_library", "", schema_url);

  auto sdk_logger = static_cast<opentelemetry::sdk::logs::Logger *>(logger.get());
  ASSERT_EQ(sdk_logger->GetInstrumentationLibrary().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger->GetInstrumentationLibrary().GetVersion(), "");
  ASSERT_EQ(sdk_logger->GetInstrumentationLibrary().GetSchemaURL(), schema_url);
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
  auto api_lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = api_lp->GetLogger("logger", "", "opentelelemtry_library", "", schema_url);

  // Cast the API LoggerProvider to an SDK Logger Provider and assert that it is still the same
  // LoggerProvider by checking that getting a logger with the same name as the previously defined
  // logger is the same instance
  auto lp      = static_cast<LoggerProvider *>(api_lp.get());
  auto logger2 = lp->GetLogger("logger", "", "opentelelemtry_library", "", schema_url);
  ASSERT_EQ(logger, logger2);

  auto sdk_logger = static_cast<opentelemetry::sdk::logs::Logger *>(logger.get());
  ASSERT_EQ(sdk_logger->GetInstrumentationLibrary().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger->GetInstrumentationLibrary().GetVersion(), "");
  ASSERT_EQ(sdk_logger->GetInstrumentationLibrary().GetSchemaURL(), schema_url);
  // Set a processor for the LoggerProvider
  auto shared_recordable = std::shared_ptr<LogRecord>(new LogRecord());
  lp->AddProcessor(std::unique_ptr<LogProcessor>(new MockProcessor(shared_recordable)));

  // Check that the recordable created by the Log() statement is set properly
  logger->Log(logs_api::Severity::kWarn, "Log Message");

  ASSERT_EQ(shared_recordable->GetSeverity(), logs_api::Severity::kWarn);
  ASSERT_EQ(shared_recordable->GetBody(), "Log Message");
}
#endif
