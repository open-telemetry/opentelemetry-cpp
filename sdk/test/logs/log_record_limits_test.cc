// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/logs/logger.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/logs/log_record_limits.h"
#include "opentelemetry/sdk/logs/logger_config.h"
#include "opentelemetry/sdk/logs/logger_provider.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/sdk/resource/resource.h"

using opentelemetry::sdk::logs::LogRecordLimits;
using opentelemetry::sdk::logs::ReadWriteLogRecord;
namespace nostd = opentelemetry::nostd;

TEST(LogRecordLimits, DefaultsPassThroughWithoutLimitsObject)
{
  ReadWriteLogRecord record;
  for (int i = 0; i < 200; ++i)
  {
    record.SetAttribute("attr_" + std::to_string(i), static_cast<int64_t>(i));
  }
  ASSERT_EQ(record.GetAttributes().size(), 200u);
  ASSERT_EQ(record.GetDroppedAttributesCount(), 0u);
}

TEST(LogRecordLimits, CountLimitDropsExcessAttributes)
{
  LogRecordLimits limits;
  limits.attribute_count_limit = 3;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("k1", static_cast<int64_t>(1));
  record.SetAttribute("k2", static_cast<int64_t>(2));
  record.SetAttribute("k3", static_cast<int64_t>(3));
  record.SetAttribute("k4", static_cast<int64_t>(4));
  record.SetAttribute("k5", static_cast<int64_t>(5));

  ASSERT_EQ(record.GetAttributes().size(), 3u);
  ASSERT_EQ(record.GetDroppedAttributesCount(), 2u);
  ASSERT_TRUE(record.GetAttributes().count("k1") == 1);
  ASSERT_TRUE(record.GetAttributes().count("k2") == 1);
  ASSERT_TRUE(record.GetAttributes().count("k3") == 1);
  ASSERT_TRUE(record.GetAttributes().count("k4") == 0);
  ASSERT_TRUE(record.GetAttributes().count("k5") == 0);
}

TEST(LogRecordLimits, CountLimitReplaceExistingKeyDoesNotDrop)
{
  LogRecordLimits limits;
  limits.attribute_count_limit = 2;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("k1", static_cast<int64_t>(1));
  record.SetAttribute("k2", static_cast<int64_t>(2));
  // Replace existing key while at limit; must not increment dropped count.
  record.SetAttribute("k1", static_cast<int64_t>(99));

  ASSERT_EQ(record.GetAttributes().size(), 2u);
  ASSERT_EQ(record.GetDroppedAttributesCount(), 0u);
  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("k1")), 99);
}

TEST(LogRecordLimits, LengthLimitTruncatesString)
{
  LogRecordLimits limits;
  limits.attribute_value_length_limit = 5;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("k", nostd::string_view("0123456789"));

  ASSERT_EQ(record.GetDroppedAttributesCount(), 0u);
  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("k")), "01234");
}

TEST(LogRecordLimits, LengthLimitTruncatesEachStringInArray)
{
  LogRecordLimits limits;
  limits.attribute_value_length_limit = 3;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  nostd::string_view values[] = {nostd::string_view("aaaaaa"), nostd::string_view("bb"),
                                 nostd::string_view("cccc")};
  record.SetAttribute("k", nostd::span<const nostd::string_view>(values, 3));

  const auto &stored = nostd::get<std::vector<std::string>>(record.GetAttributes().at("k"));
  ASSERT_EQ(stored.size(), 3u);
  ASSERT_EQ(stored[0], "aaa");
  ASSERT_EQ(stored[1], "bb");
  ASSERT_EQ(stored[2], "ccc");
}

TEST(LogRecordLimits, LengthLimitDoesNotAffectNonStringTypes)
{
  LogRecordLimits limits;
  limits.attribute_value_length_limit = 1;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("i", static_cast<int64_t>(1234567890));
  record.SetAttribute("d", 3.14);
  record.SetAttribute("b", true);

  ASSERT_EQ(nostd::get<int64_t>(record.GetAttributes().at("i")), 1234567890);
  ASSERT_DOUBLE_EQ(nostd::get<double>(record.GetAttributes().at("d")), 3.14);
  ASSERT_EQ(nostd::get<bool>(record.GetAttributes().at("b")), true);
}

TEST(LogRecordLimits, LengthLimitShorterThanValueLeavesValueAlone)
{
  LogRecordLimits limits;
  limits.attribute_value_length_limit = 100;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("k", nostd::string_view("short"));

  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("k")), "short");
}

TEST(LogRecordLimits, CountAndLengthCombined)
{
  LogRecordLimits limits;
  limits.attribute_count_limit        = 2;
  limits.attribute_value_length_limit = 4;
  ReadWriteLogRecord record;
  record.SetLogRecordLimits(limits);

  record.SetAttribute("k1", nostd::string_view("abcdefgh"));
  record.SetAttribute("k2", nostd::string_view("12345678"));
  record.SetAttribute("k3", nostd::string_view("ignored"));

  ASSERT_EQ(record.GetAttributes().size(), 2u);
  ASSERT_EQ(record.GetDroppedAttributesCount(), 1u);
  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("k1")), "abcd");
  ASSERT_EQ(nostd::get<std::string>(record.GetAttributes().at("k2")), "1234");
}

namespace
{

// Minimal Recordable that records whether SetLogRecordLimits was called and
// what limits arrived. Stubs every other Recordable method.
class TrackingRecordable final : public opentelemetry::sdk::logs::Recordable
{
public:
  int set_limits_call_count               = 0;
  std::size_t captured_count_limit        = 0;
  std::size_t captured_value_length_limit = 0;

  void SetLogRecordLimits(const LogRecordLimits &limits) noexcept override
  {
    ++set_limits_call_count;
    captured_count_limit        = limits.attribute_count_limit;
    captured_value_length_limit = limits.attribute_value_length_limit;
  }

  void SetResource(const opentelemetry::sdk::resource::Resource &) noexcept override {}
  void SetInstrumentationScope(
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &) noexcept override
  {}
  void SetTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}
  void SetObservedTimestamp(opentelemetry::common::SystemTimestamp) noexcept override {}
  void SetSeverity(opentelemetry::logs::Severity) noexcept override {}
  void SetBody(const opentelemetry::common::AttributeValue &) noexcept override {}
  void SetEventId(int64_t, nostd::string_view) noexcept override {}
  void SetTraceId(const opentelemetry::trace::TraceId &) noexcept override {}
  void SetSpanId(const opentelemetry::trace::SpanId &) noexcept override {}
  void SetTraceFlags(const opentelemetry::trace::TraceFlags &) noexcept override {}
  void SetAttribute(nostd::string_view,
                    const opentelemetry::common::AttributeValue &) noexcept override
  {}
};

// Processor that hands out TrackingRecordable instances and exposes a pointer
// to the most recently produced one for inspection.
class TrackingProcessor final : public opentelemetry::sdk::logs::LogRecordProcessor
{
public:
  TrackingRecordable *last = nullptr;

  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override
  {
    auto recordable = std::unique_ptr<TrackingRecordable>(new TrackingRecordable());
    last            = recordable.get();
    return recordable;
  }

  void OnEmit(std::unique_ptr<opentelemetry::sdk::logs::Recordable> &&) noexcept override {}

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
};

}  // namespace

TEST(LogRecordLimitsWiring, LoggerForwardsContextLimitsToRecordable)
{
  using opentelemetry::sdk::instrumentationscope::ScopeConfigurator;
  using opentelemetry::sdk::logs::LoggerConfig;
  using opentelemetry::sdk::logs::LoggerProviderFactory;
  using opentelemetry::sdk::logs::LogRecordProcessor;

  LogRecordLimits limits;
  limits.attribute_count_limit        = 7;
  limits.attribute_value_length_limit = 42;

  std::vector<std::unique_ptr<LogRecordProcessor>> processors;
  auto tracking_processor = std::unique_ptr<TrackingProcessor>(new TrackingProcessor());
  auto *tracker_ptr       = tracking_processor.get();
  processors.push_back(std::move(tracking_processor));

  auto configurator = std::make_unique<ScopeConfigurator<LoggerConfig>>(
      ScopeConfigurator<LoggerConfig>::Builder(LoggerConfig::Default()).Build());

  auto provider = LoggerProviderFactory::Create(std::move(processors),
                                                opentelemetry::sdk::resource::Resource::Create({}),
                                                std::move(configurator), limits);

  auto logger = provider->GetLogger("wiring_test", "");
  auto record = logger->CreateLogRecord();

  ASSERT_NE(tracker_ptr->last, nullptr);
  EXPECT_EQ(tracker_ptr->last->set_limits_call_count, 1);
  EXPECT_EQ(tracker_ptr->last->captured_count_limit, 7u);
  EXPECT_EQ(tracker_ptr->last->captured_value_length_limit, 42u);
}
