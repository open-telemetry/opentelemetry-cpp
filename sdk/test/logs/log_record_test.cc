// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <stdint.h>
#include <algorithm>
#include <string>
#include <vector>

#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <gtest/gtest.h>

using opentelemetry::sdk::logs::ReadWriteLogRecord;
namespace trace_api = opentelemetry::trace;
namespace logs_api  = opentelemetry::logs;
namespace nostd     = opentelemetry::nostd;

// Test what a default ReadWriteLogRecord with no fields set holds
TEST(ReadWriteLogRecord, GetDefaultValues)
{
  trace_api::TraceId zero_trace_id;
  trace_api::SpanId zero_span_id;
  trace_api::TraceFlags zero_trace_flags;
  ReadWriteLogRecord record;

  ASSERT_EQ(record.GetSeverity(), logs_api::Severity::kInvalid);
  ASSERT_NE(record.GetResource().GetAttributes().size(), 0);
  ASSERT_EQ(record.GetAttributes().size(), 0);
  ASSERT_EQ(record.GetTraceId(), zero_trace_id);
  ASSERT_EQ(record.GetSpanId(), zero_span_id);
  ASSERT_EQ(record.GetTraceFlags(), zero_trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), std::chrono::nanoseconds(0));
}

// Test ReadWriteLogRecord fields are properly set and get
TEST(ReadWriteLogRecord, SetAndGet)
{
  trace_api::TraceId trace_id;
  trace_api::SpanId span_id;
  trace_api::TraceFlags trace_flags;
  opentelemetry::common::SystemTimestamp now(std::chrono::system_clock::now());

  // Set most fields of the ReadWriteLogRecord
  ReadWriteLogRecord record;
  auto resource = opentelemetry::sdk::resource::Resource::Create({{"res1", true}});
  record.SetSeverity(logs_api::Severity::kInvalid);
  record.SetBody("Message");
  record.SetResource(resource);
  record.SetAttribute("attr1", (int64_t)314159);
  record.SetTraceId(trace_id);
  record.SetSpanId(span_id);
  record.SetTraceFlags(trace_flags);
  record.SetTimestamp(now);

  // Test that all fields match what was set
  ASSERT_EQ(record.GetSeverity(), logs_api::Severity::kInvalid);
  if (nostd::holds_alternative<const char *>(record.GetBody()))
  {
    ASSERT_EQ(std::string(nostd::get<const char *>(record.GetBody())), "Message");
  }
  else if (nostd::holds_alternative<nostd::string_view>(record.GetBody()))
  {
    ASSERT_TRUE(nostd::get<nostd::string_view>(record.GetBody()) == "Message");
  }
  ASSERT_TRUE(nostd::get<bool>(record.GetResource().GetAttributes().at("res1")));
  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("attr1")), 314159);
  ASSERT_EQ(record.GetTraceId(), trace_id);
  ASSERT_EQ(record.GetSpanId(), span_id);
  ASSERT_EQ(record.GetTraceFlags(), trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), now.time_since_epoch());
}

// Define a basic Logger class
class TestBodyLogger : public opentelemetry::logs::Logger
{
public:
  const nostd::string_view GetName() noexcept override { return "test body logger"; }

  nostd::unique_ptr<opentelemetry::logs::LogRecord> CreateLogRecord() noexcept override
  {
    return nostd::unique_ptr<opentelemetry::logs::LogRecord>(new ReadWriteLogRecord());
  }

  using opentelemetry::logs::Logger::EmitLogRecord;

  void EmitLogRecord(nostd::unique_ptr<opentelemetry::logs::LogRecord> &&record) noexcept override
  {
    if (record)
    {
      last_body_ = static_cast<ReadWriteLogRecord *>(record.get())->GetBody();
    }
  }

  const opentelemetry::common::AttributeValue &GetLastLogRecord() const noexcept
  {
    return last_body_;
  }

private:
  opentelemetry::common::AttributeValue last_body_;
};

// Define a basic LoggerProvider class that returns an instance of the logger class defined above
class TestBodyProvider : public opentelemetry::logs::LoggerProvider
{
public:
  using opentelemetry::logs::LoggerProvider::GetLogger;

  nostd::shared_ptr<opentelemetry::logs::Logger> GetLogger(
      nostd::string_view /* logger_name */,
      nostd::string_view /* library_name */,
      nostd::string_view /* library_version */,
      nostd::string_view /* schema_url */,
      const opentelemetry::common::KeyValueIterable & /* attributes */) override
  {
    return nostd::shared_ptr<opentelemetry::logs::Logger>(new TestBodyLogger());
  }
};

TEST(LogBody, BodyConversation)
{
  // Push the new loggerprovider class into the global singleton
  TestBodyProvider lp;

  // Check that the implementation was pushed by calling TestLogger's GetName()
  nostd::string_view schema_url{"https://opentelemetry.io/schemas/1.11.0"};
  auto logger = lp.GetLogger("TestBodyProvider", "opentelelemtry_library", "", schema_url, {});

  auto real_logger = static_cast<TestBodyLogger *>(logger.get());

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, true);
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<bool>(real_logger->GetLastLogRecord()));
  ASSERT_TRUE(opentelemetry::nostd::get<bool>(real_logger->GetLastLogRecord()));

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, static_cast<int32_t>(123));
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int32_t>(real_logger->GetLastLogRecord()));
  ASSERT_EQ(123, opentelemetry::nostd::get<int32_t>(real_logger->GetLastLogRecord()));

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, static_cast<uint32_t>(124));
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<uint32_t>(real_logger->GetLastLogRecord()));
  ASSERT_EQ(124, opentelemetry::nostd::get<uint32_t>(real_logger->GetLastLogRecord()));

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, static_cast<int64_t>(125));
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<int64_t>(real_logger->GetLastLogRecord()));
  ASSERT_EQ(125, opentelemetry::nostd::get<int64_t>(real_logger->GetLastLogRecord()));

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, static_cast<uint64_t>(126));
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<uint64_t>(real_logger->GetLastLogRecord()));
  ASSERT_EQ(126, opentelemetry::nostd::get<uint64_t>(real_logger->GetLastLogRecord()));

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, static_cast<double>(127.0));
  ASSERT_TRUE(opentelemetry::nostd::holds_alternative<double>(real_logger->GetLastLogRecord()));
  ASSERT_TRUE(std::abs(127.0 - opentelemetry::nostd::get<double>(real_logger->GetLastLogRecord())) <
              0.0001);

  real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, "128");
  ASSERT_TRUE(
      opentelemetry::nostd::holds_alternative<const char *>(real_logger->GetLastLogRecord()) ||
      opentelemetry::nostd::holds_alternative<nostd::string_view>(real_logger->GetLastLogRecord()));
  if (opentelemetry::nostd::holds_alternative<const char *>(real_logger->GetLastLogRecord()))
  {
    ASSERT_EQ(nostd::string_view{"128"},
              opentelemetry::nostd::get<const char *>(real_logger->GetLastLogRecord()));
  }
  else
  {
    ASSERT_EQ(nostd::string_view{"128"},
              opentelemetry::nostd::get<nostd::string_view>(real_logger->GetLastLogRecord()));
  }

  {
    bool data[]                       = {true, false, true};
    nostd::span<const bool> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const bool>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const bool> output =
        opentelemetry::nostd::get<nostd::span<const bool>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }

  {
    int32_t data[]                       = {221, 222, 223};
    nostd::span<const int32_t> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const int32_t>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const int32_t> output =
        opentelemetry::nostd::get<nostd::span<const int32_t>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }

  {
    uint32_t data[]                       = {231, 232, 233};
    nostd::span<const uint32_t> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const uint32_t>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const uint32_t> output =
        opentelemetry::nostd::get<nostd::span<const uint32_t>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }

  {
    int64_t data[]                       = {241, 242, 243};
    nostd::span<const int64_t> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const int64_t>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const int64_t> output =
        opentelemetry::nostd::get<nostd::span<const int64_t>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }

  {
    uint64_t data[]                       = {251, 252, 253};
    nostd::span<const uint64_t> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const uint64_t>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const uint64_t> output =
        opentelemetry::nostd::get<nostd::span<const uint64_t>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }

  {
    uint8_t data[]                       = {161, 162, 163};
    nostd::span<const uint8_t> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const uint8_t>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const uint8_t> output =
        opentelemetry::nostd::get<nostd::span<const uint8_t>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }

  {
    double data[]                       = {271.0, 272.0, 273.0};
    nostd::span<const double> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const double>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const double> output =
        opentelemetry::nostd::get<nostd::span<const double>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_TRUE(std::abs(data_span[i] - output[i]) < 0.0001);
    }
  }

  {
    std::string data_origin[] = {"281", "282", "283"};
    nostd::string_view data[] = {data_origin[0], data_origin[1], data_origin[2]};
    nostd::span<const nostd::string_view> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<nostd::span<const nostd::string_view>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const nostd::string_view> output =
        opentelemetry::nostd::get<nostd::span<const nostd::string_view>>(
            real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }
}
