// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include <string>

#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/logs/logger.h"
#  include "opentelemetry/sdk/logs/recordable.h"

#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;
namespace nostd    = opentelemetry::nostd;

TEST(LoggerSDK, LogToNullProcessor)
{
  // Confirm Logger::Log() does not have undefined behavior
  // even when there is no processor set
  // since it calls Processor::OnEmit()

  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp->GetLogger("logger", "", "opentelelemtry_library", "", schema_url);

  auto sdk_logger = static_cast<opentelemetry::sdk::logs::Logger *>(logger.get());
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetVersion(), "");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetSchemaURL(), schema_url);
  // Log a sample log record to a nullptr processor
  logger->Debug("Test log");
}

class MockLogRecordable final : public opentelemetry::sdk::logs::Recordable
{
public:
  void SetTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}

  void SetObservedTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}

  opentelemetry::logs::Severity GetSeverity() const noexcept { return severity_; }

  void SetSeverity(opentelemetry::logs::Severity severity) noexcept override
  {
    severity_ = severity;
  }

  nostd::string_view GetBody() const noexcept { return body_; }

  void SetBody(const opentelemetry::common::AttributeValue &message) noexcept override
  {
    if (nostd::holds_alternative<const char *>(message))
    {
      body_ = nostd::get<const char *>(message);
    }
    else if (nostd::holds_alternative<nostd::string_view>(message))
    {
      body_ = static_cast<std::string>(nostd::get<nostd::string_view>(message));
    }
  }

  void SetBody(const std::string &message) noexcept { body_ = message; }

  void SetTraceId(const opentelemetry::trace::TraceId &) noexcept override {}

  void SetSpanId(const opentelemetry::trace::SpanId &) noexcept override {}

  void SetTraceFlags(const opentelemetry::trace::TraceFlags &) noexcept override {}

  void SetAttribute(nostd::string_view,
                    const opentelemetry::common::AttributeValue &) noexcept override
  {}

  void SetResource(const opentelemetry::sdk::resource::Resource &) noexcept override {}

  void SetInstrumentationScope(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &) noexcept override
  {}

private:
  opentelemetry::logs::Severity severity_ = opentelemetry::logs::Severity::kInvalid;
  std::string body_;
};

class MockProcessor final : public LogRecordProcessor
{
private:
  std::shared_ptr<MockLogRecordable> record_received_;

public:
  // A processor used for testing that keeps a track of the recordable it received
  explicit MockProcessor(std::shared_ptr<MockLogRecordable> record_received) noexcept
      : record_received_(record_received)
  {}

  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<opentelemetry::sdk::logs::Recordable>(new MockLogRecordable());
  }

  // OnEmit stores the record it receives into the shared_ptr recordable passed into its
  // constructor
  void OnEmit(std::unique_ptr<opentelemetry::sdk::logs::Recordable> &&record) noexcept override
  {
    // Cast the recordable received into a concrete MockLogRecordable type
    auto copy =
        std::shared_ptr<MockLogRecordable>(static_cast<MockLogRecordable *>(record.release()));

    // Copy over the received log record's severity, name, and body fields over to the recordable
    // passed in the constructor
    record_received_->SetSeverity(copy->GetSeverity());
    record_received_->SetBody(copy->GetBody());
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }
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
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetVersion(), "");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetSchemaURL(), schema_url);
  // Set a processor for the LoggerProvider
  auto shared_recordable = std::shared_ptr<MockLogRecordable>(new MockLogRecordable());
  lp->AddProcessor(std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>(
      new MockProcessor(shared_recordable)));

  // Check that the recordable created by the Log() statement is set properly
  logger->Log(logs_api::Severity::kWarn, "Log Message");

  ASSERT_EQ(shared_recordable->GetSeverity(), logs_api::Severity::kWarn);
  ASSERT_EQ(shared_recordable->GetBody(), "Log Message");
}
#endif
