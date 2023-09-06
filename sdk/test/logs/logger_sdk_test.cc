// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <string>

#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/logs/event_logger.h"
#include "opentelemetry/sdk/logs/event_logger_provider.h"
#include "opentelemetry/sdk/logs/logger.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/tracer.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;
namespace nostd    = opentelemetry::nostd;

TEST(LoggerSDK, LogToNullProcessor)
{
  // Confirm Logger::EmitLogRecord() does not have undefined behavior
  // even when there is no processor set
  // since it calls Processor::OnEmit()

  auto lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp->GetLogger("logger", "opentelelemtry_library", "", schema_url);

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

  void SetObservedTimestamp(
      opentelemetry::common::SystemTimestamp observed_timestamp) noexcept override
  {
    observed_timestamp_ = observed_timestamp;
  }

  const opentelemetry::common::SystemTimestamp &GetObservedTimestamp() const noexcept
  {
    return observed_timestamp_;
  }

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

  void SetEventId(int64_t id, nostd::string_view name) noexcept override
  {
    event_id_              = id;
    log_record_event_name_ = static_cast<std::string>(name);
  }

  void SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept override
  {
    trace_id_ = trace_id;
  }
  inline const opentelemetry::trace::TraceId &GetTraceId() const noexcept { return trace_id_; }

  void SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept override
  {
    span_id_ = span_id;
  }

  inline const opentelemetry::trace::SpanId &GetSpanId() const noexcept { return span_id_; }

  void SetTraceFlags(const opentelemetry::trace::TraceFlags &trace_flags) noexcept override
  {
    trace_flags_ = trace_flags;
  }

  inline const opentelemetry::trace::TraceFlags &GetTraceFlags() const noexcept
  {
    return trace_flags_;
  }

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override
  {
    if (!nostd::holds_alternative<nostd::string_view>(value))
    {
      return;
    }

    if (key == "event.domain")
    {
      event_domain_ = static_cast<std::string>(nostd::get<nostd::string_view>(value));
    }
    else if (key == "event.name")
    {
      event_name_ = static_cast<std::string>(nostd::get<nostd::string_view>(value));
    }
  }

  inline const std::string &GetEventName() const noexcept { return event_name_; }

  inline const std::string &GetEventDomain() const noexcept { return event_domain_; }

  void SetResource(const opentelemetry::sdk::resource::Resource &) noexcept override {}

  void SetInstrumentationScope(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &) noexcept override
  {}

  void CopyFrom(const MockLogRecordable &other)
  {
    severity_           = other.severity_;
    body_               = other.body_;
    trace_id_           = other.trace_id_;
    span_id_            = other.span_id_;
    trace_flags_        = other.trace_flags_;
    event_name_         = other.event_name_;
    event_domain_       = other.event_domain_;
    observed_timestamp_ = other.observed_timestamp_;
  }

private:
  opentelemetry::logs::Severity severity_ = opentelemetry::logs::Severity::kInvalid;
  std::string body_;
  int64_t event_id_;
  std::string log_record_event_name_;
  opentelemetry::trace::TraceId trace_id_;
  opentelemetry::trace::SpanId span_id_;
  opentelemetry::trace::TraceFlags trace_flags_;
  std::string event_name_;
  std::string event_domain_;
  opentelemetry::common::SystemTimestamp observed_timestamp_ =
      std::chrono::system_clock::from_time_t(0);
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
    record_received_->CopyFrom(*copy);
  }

  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }
};

TEST(LoggerSDK, LogToAProcessor)
{
  // Create an API LoggerProvider and logger
  auto api_lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = api_lp->GetLogger("logger", "opentelelemtry_library", "", schema_url);

  // Cast the API LoggerProvider to an SDK Logger Provider and assert that it is still the same
  // LoggerProvider by checking that getting a logger with the same name as the previously defined
  // logger is the same instance
  auto lp      = static_cast<LoggerProvider *>(api_lp.get());
  auto logger2 = lp->GetLogger("logger", "opentelelemtry_library", "", schema_url);
  ASSERT_EQ(logger, logger2);

  nostd::shared_ptr<opentelemetry::trace::Span> include_span;
  {
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> span_processors;
    auto trace_provider =
        opentelemetry::sdk::trace::TracerProviderFactory::Create(std::move(span_processors));
    include_span = trace_provider->GetTracer("logger")->StartSpan("test_logger");
  }
  opentelemetry::trace::Scope trace_scope{include_span};

  auto now = std::chrono::system_clock::now();

  auto sdk_logger = static_cast<opentelemetry::sdk::logs::Logger *>(logger.get());
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetVersion(), "");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetSchemaURL(), schema_url);
  // Set a processor for the LoggerProvider
  auto shared_recordable = std::shared_ptr<MockLogRecordable>(new MockLogRecordable());
  lp->AddProcessor(std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>(
      new MockProcessor(shared_recordable)));

  // Check that the recordable created by the EmitLogRecord() statement is set properly
  logger->EmitLogRecord(logs_api::Severity::kWarn, "Log Message");

  ASSERT_EQ(shared_recordable->GetSeverity(), logs_api::Severity::kWarn);
  ASSERT_EQ(shared_recordable->GetBody(), "Log Message");
  ASSERT_EQ(shared_recordable->GetTraceFlags().flags(),
            include_span->GetContext().trace_flags().flags());
  char trace_id_in_logger[opentelemetry::trace::TraceId::kSize * 2];
  char trace_id_in_span[opentelemetry::trace::TraceId::kSize * 2];
  char span_id_in_logger[opentelemetry::trace::SpanId::kSize * 2];
  char span_id_in_span[opentelemetry::trace::SpanId::kSize * 2];
  shared_recordable->GetTraceId().ToLowerBase16(trace_id_in_logger);
  include_span->GetContext().trace_id().ToLowerBase16(trace_id_in_span);
  shared_recordable->GetSpanId().ToLowerBase16(span_id_in_logger);
  include_span->GetContext().span_id().ToLowerBase16(span_id_in_span);
  std::string trace_id_text_in_logger{trace_id_in_logger, sizeof(trace_id_in_logger)};
  std::string trace_id_text_in_span{trace_id_in_span, sizeof(trace_id_in_span)};
  std::string span_id_text_in_logger{span_id_in_logger, sizeof(span_id_in_logger)};
  std::string span_id_text_in_span{span_id_in_span, sizeof(span_id_in_span)};
  ASSERT_EQ(trace_id_text_in_logger, trace_id_text_in_span);
  ASSERT_EQ(span_id_text_in_logger, span_id_text_in_span);

  ASSERT_GE(
      static_cast<std::chrono::system_clock::time_point>(shared_recordable->GetObservedTimestamp()),
      now);
}

TEST(LoggerSDK, EventLog)
{
  // Create an API LoggerProvider and logger
  auto api_lp = std::shared_ptr<logs_api::LoggerProvider>(new LoggerProvider());
  const std::string schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = api_lp->GetLogger("logger", "opentelelemtry_library", "", schema_url);

  auto api_elp      = std::shared_ptr<logs_api::EventLoggerProvider>(new EventLoggerProvider());
  auto event_logger = api_elp->CreateEventLogger(logger, "otel-cpp.event_domain");

  auto sdk_logger = static_cast<opentelemetry::sdk::logs::Logger *>(logger.get());
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetName(), "opentelelemtry_library");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetVersion(), "");
  ASSERT_EQ(sdk_logger->GetInstrumentationScope().GetSchemaURL(), schema_url);
  // Set a processor for the LoggerProvider
  auto shared_recordable = std::shared_ptr<MockLogRecordable>(new MockLogRecordable());
  auto sdk_lp            = static_cast<LoggerProvider *>(api_lp.get());
  sdk_lp->AddProcessor(std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor>(
      new MockProcessor(shared_recordable)));

  // Check that the recordable created by the EmitEvent() statement is set properly
  event_logger->EmitEvent("otel-cpp.event_name", logs_api::Severity::kWarn, "Event Log Message");

  ASSERT_EQ(shared_recordable->GetSeverity(), logs_api::Severity::kWarn);
  ASSERT_EQ(shared_recordable->GetBody(), "Event Log Message");
  ASSERT_EQ(shared_recordable->GetEventName(), "otel-cpp.event_name");
  ASSERT_EQ(shared_recordable->GetEventDomain(), "otel-cpp.event_domain");
}
