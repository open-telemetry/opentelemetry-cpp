// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <chrono>
#include <cstdlib>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/sdk/logs/multi_recordable.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"

using opentelemetry::sdk::logs::ReadWriteLogRecord;
namespace trace_api = opentelemetry::trace;
namespace logs_api  = opentelemetry::logs;
namespace logs_sdk  = opentelemetry::sdk::logs;
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
  record.SetAttribute("attr1", static_cast<int64_t>(314159));
  record.SetTraceId(trace_id);
  record.SetSpanId(span_id);
  record.SetTraceFlags(trace_flags);
  record.SetTimestamp(now);

  // Test that all fields match what was set
  ASSERT_EQ(record.GetSeverity(), logs_api::Severity::kInvalid);
  if (nostd::holds_alternative<std::string>(record.GetBody()))
  {
    ASSERT_EQ(std::string(nostd::get<std::string>(record.GetBody())), "Message");
  }
  ASSERT_TRUE(nostd::get<bool>(record.GetResource().GetAttributes().at("res1")));
  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("attr1")), 314159);
  ASSERT_EQ(record.GetTraceId(), trace_id);
  ASSERT_EQ(record.GetSpanId(), span_id);
  ASSERT_EQ(record.GetTraceFlags(), trace_flags);
  ASSERT_EQ(record.GetTimestamp().time_since_epoch(), now.time_since_epoch());
}

TEST(ReadWriteLogRecord, SetAttributeAppliesAttributeValueLengthLimit)
{
  logs_sdk::LogRecordLimits limits;
  limits.attribute_value_length_limit = 3;

  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);
  record.SetAttribute("string", nostd::string_view("abcdef"));
  record.SetAttribute("cstring", "ghijkl");
  record.SetAttribute("int", static_cast<int64_t>(314159));

  uint8_t byte_values[] = {1, 2, 3, 4, 5};
  record.SetAttribute("bytes", nostd::span<const uint8_t>(byte_values));

  nostd::string_view string_values[] = {"abcdef", "xy", "1234"};
  record.SetAttribute("strings", nostd::span<const nostd::string_view>(string_values));

  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("string")), "abc");
  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("cstring")), "ghi");
  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("int")), 314159);

  const auto byte_output = nostd::get<std::vector<uint8_t>>(record.GetAttributes().at("bytes"));
  ASSERT_EQ(byte_output, std::vector<uint8_t>({1, 2, 3}));

  const auto string_output =
      nostd::get<std::vector<std::string>>(record.GetAttributes().at("strings"));
  ASSERT_EQ(string_output, std::vector<std::string>({"abc", "xy", "123"}));
}

TEST(ReadWriteLogRecord, SetAttributeAppliesZeroAttributeValueLengthLimit)
{
  logs_sdk::LogRecordLimits limits;
  limits.attribute_value_length_limit = 0;

  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);
  record.SetAttribute("string", nostd::string_view("abcdef"));

  uint8_t byte_values[] = {1, 2, 3};
  record.SetAttribute("bytes", nostd::span<const uint8_t>(byte_values));

  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("string")), "");
  ASSERT_TRUE(nostd::get<std::vector<uint8_t>>(record.GetAttributes().at("bytes")).empty());
}

TEST(ReadWriteLogRecord, SetAttributeTruncatesUtf8ByCodePoint)
{
  logs_sdk::LogRecordLimits limits;
  limits.attribute_value_length_limit = 3;

  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  const std::string ga    = "\xEA\xB0\x80";               // '가' is 1 code point, 3 bytes
  const std::string na    = "\xEB\x82\x98";               // '나' is 1 code point, 3 bytes
  const std::string da    = "\xEB\x8B\xA4";               // '다' is 1 code point, 3 bytes
  const std::string three = ga + na + da;                 // 3 code points, 9 bytes
  const std::string six   = ga + na + da + ga + na + da;  // 6 code points, 18 bytes

  record.SetAttribute("sv", nostd::string_view(six));
  record.SetAttribute("cstr", six.c_str());

  nostd::string_view strings[] = {nostd::string_view(six), nostd::string_view(ga)};
  record.SetAttribute("strings", nostd::span<const nostd::string_view>(strings));

  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("sv")), three);
  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("cstr")), three);

  const auto strings_output =
      nostd::get<std::vector<std::string>>(record.GetAttributes().at("strings"));
  ASSERT_EQ(strings_output, std::vector<std::string>({three, ga}));
}

TEST(ReadWriteLogRecord, SetAttributeTruncatesInvalidUtf8ByByte)
{
  logs_sdk::LogRecordLimits limits;
  limits.attribute_value_length_limit = 1;

  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  // A 3-byte lead (0xE0) followed by ASCII is not valid UTF-8 (no continuation bytes), so it must
  // degrade to byte truncation: a 1-character limit keeps exactly the lead byte, not the trailing
  // ASCII swallowed as fake continuations.
  const std::string bad_lead = std::string("\xE0", 1) + "AB";  // 0xE0 'A' 'B'
  // 0xE0 with the sequence running past the end must also keep 1 byte (not truncate to empty).
  const std::string truncated_lead = std::string("\xE0", 1) + "A";  // 0xE0 'A'

  record.SetAttribute("bad_lead", nostd::string_view(bad_lead));
  record.SetAttribute("truncated_lead", nostd::string_view(truncated_lead));

  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("bad_lead")), std::string("\xE0", 1));
  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("truncated_lead")),
            std::string("\xE0", 1));
}

TEST(ReadWriteLogRecord, SetAttributeAppliesAttributeCountLimit)
{
  logs_sdk::LogRecordLimits limits;
  limits.attribute_count_limit = 3;

  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("a", static_cast<int64_t>(1));
  record.SetAttribute("b", static_cast<int64_t>(2));
  record.SetAttribute("c", static_cast<int64_t>(3));
  record.SetAttribute("d", static_cast<int64_t>(4));  // 4th distinct key -> discarded
  record.SetAttribute("e", static_cast<int64_t>(5));  // 5th distinct key -> discarded

  ASSERT_EQ(record.GetAttributes().size(), 3u);
  ASSERT_EQ(record.GetAttributeDroppedCount(), 2u);
  ASSERT_EQ(record.GetAttributes().count("d"), 0u);
  ASSERT_EQ(record.GetAttributes().count("e"), 0u);

  // Re-setting an existing key overwrites its value and does NOT count toward the limit or drop.
  record.SetAttribute("a", static_cast<int64_t>(99));
  ASSERT_EQ(record.GetAttributes().size(), 3u);
  ASSERT_EQ(record.GetAttributeDroppedCount(), 2u);
  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("a")), 99);
}

namespace
{
class CountLimitStubProcessor final : public logs_sdk::LogRecordProcessor
{
public:
  std::unique_ptr<logs_sdk::Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<logs_sdk::Recordable>(new ReadWriteLogRecord());
  }
  void OnEmit(std::unique_ptr<logs_sdk::Recordable> &&record) noexcept override
  {
    auto record_ptr = std::move(record);
  }
  bool ForceFlush(std::chrono::microseconds /* timeout */) noexcept override { return true; }
  bool Shutdown(std::chrono::microseconds /* timeout */) noexcept override { return true; }
};
}  // namespace

TEST(MultiRecordable, ReleaseRecordablePropagatesDroppedAttributeCount)
{
  CountLimitStubProcessor processor_a;
  CountLimitStubProcessor processor_b;

  logs_sdk::MultiRecordable multi;
  multi.AddRecordable(processor_a, std::unique_ptr<logs_sdk::Recordable>(new ReadWriteLogRecord()));
  multi.AddRecordable(processor_b, std::unique_ptr<logs_sdk::Recordable>(new ReadWriteLogRecord()));

  logs_sdk::LogRecordLimits limits;
  limits.attribute_count_limit = 2;
  multi.SetLogRecordLimits(limits);

  multi.SetAttribute("a", static_cast<int64_t>(1));
  multi.SetAttribute("b", static_cast<int64_t>(2));
  multi.SetAttribute("c", static_cast<int64_t>(3));  // 3rd distinct key -> dropped
  multi.SetAttribute("d", static_cast<int64_t>(4));  // 4th distinct key -> dropped
  ASSERT_EQ(multi.GetAttributeDroppedCount(), 2u);

  auto sub_a = multi.ReleaseRecordable(processor_a);
  auto sub_b = multi.ReleaseRecordable(processor_b);
  ASSERT_TRUE(sub_a);
  ASSERT_TRUE(sub_b);
  EXPECT_EQ(sub_a->GetAttributeDroppedCount(), 2u);
  EXPECT_EQ(sub_b->GetAttributeDroppedCount(), 2u);
}

// Define a basic Logger class
class TestBodyLogger : public opentelemetry::logs::Logger
{
public:
  TestBodyLogger() noexcept
  {
    SetMinimumSeverity(static_cast<uint8_t>(opentelemetry::logs::Severity::kTrace));
  }

  const nostd::string_view GetName() noexcept override { return "test body logger"; }

  using opentelemetry::logs::Logger::CreateLogRecord;

  nostd::unique_ptr<opentelemetry::logs::LogRecord> CreateLogRecord() noexcept override
  {
    return nostd::unique_ptr<opentelemetry::logs::LogRecord>(new ReadWriteLogRecord());
  }

  using opentelemetry::logs::Logger::EmitLogRecord;

  void EmitLogRecord(nostd::unique_ptr<opentelemetry::logs::LogRecord> &&record) noexcept override
  {
    auto log_record = std::move(record);
    if (log_record)
    {
      last_body_ = static_cast<ReadWriteLogRecord *>(log_record.get())->GetBody();
    }
  }

  const opentelemetry::sdk::common::OwnedAttributeValue &GetLastLogRecord() const noexcept
  {
    return last_body_;
  }

protected:
  using opentelemetry::logs::Logger::EnabledImplementation;

  bool EnabledImplementation(opentelemetry::logs::Severity /*severity*/,
                             int64_t /*event_id*/) const noexcept override
  {
    return true;
  }

  bool EnabledImplementation(
      opentelemetry::logs::Severity /*severity*/,
      const opentelemetry::logs::EventId & /*event_id*/) const noexcept override
  {
    return true;
  }

private:
  opentelemetry::sdk::common::OwnedAttributeValue last_body_;
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
      opentelemetry::nostd::holds_alternative<std::string>(real_logger->GetLastLogRecord()));
  ASSERT_EQ(nostd::string_view{"128"},
            opentelemetry::nostd::get<std::string>(real_logger->GetLastLogRecord()));

  {
    bool data[]                       = {true, false, true};
    nostd::span<const bool> data_span = data;
    real_logger->EmitLogRecord(opentelemetry::logs::Severity::kInfo, data_span);
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<bool>>(
        real_logger->GetLastLogRecord()));

    std::vector<bool> output =
        opentelemetry::nostd::get<std::vector<bool>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<int32_t>>(
        real_logger->GetLastLogRecord()));

    nostd::span<const int32_t> output =
        opentelemetry::nostd::get<std::vector<int32_t>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint32_t>>(
        real_logger->GetLastLogRecord()));

    std::vector<uint32_t> output =
        opentelemetry::nostd::get<std::vector<uint32_t>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<int64_t>>(
        real_logger->GetLastLogRecord()));

    std::vector<int64_t> output =
        opentelemetry::nostd::get<std::vector<int64_t>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint64_t>>(
        real_logger->GetLastLogRecord()));

    std::vector<uint64_t> output =
        opentelemetry::nostd::get<std::vector<uint64_t>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<uint8_t>>(
        real_logger->GetLastLogRecord()));

    std::vector<uint8_t> output =
        opentelemetry::nostd::get<std::vector<uint8_t>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<double>>(
        real_logger->GetLastLogRecord()));

    std::vector<double> output =
        opentelemetry::nostd::get<std::vector<double>>(real_logger->GetLastLogRecord());

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
    ASSERT_TRUE(opentelemetry::nostd::holds_alternative<std::vector<std::string>>(
        real_logger->GetLastLogRecord()));

    std::vector<std::string> output =
        opentelemetry::nostd::get<std::vector<std::string>>(real_logger->GetLastLogRecord());

    ASSERT_EQ(data_span.size(), output.size());

    for (size_t i = 0; i < data_span.size(); ++i)
    {
      ASSERT_EQ(data_span[i], output[i]);
    }
  }
}
