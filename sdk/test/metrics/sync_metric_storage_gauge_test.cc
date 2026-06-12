// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/common/attribute_value.h"  // IWYU pragma: keep
#include "opentelemetry/nostd/utility.h"           // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/instruments.h"

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include <stddef.h>
#  include <stdint.h>
#  include <chrono>
#  include <map>
#  include <memory>
#  include <utility>
#  include <vector>
#  include "common.h"

#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/nostd/function_ref.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#  include "opentelemetry/sdk/metrics/state/metric_collector.h"
#  include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#endif

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;

class WritableMetricStorageTestFixture : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(WritableMetricStorageTestFixture, LongGaugeLastValueAggregation)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kGauge,
                                        InstrumentValueType::kLong};
  std::map<std::string, std::string> attributes_roomA = {{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  int64_t bg_noise_level_1_roomA = 10;
  int64_t bg_noise_level_1_roomB = 20;

  storage.RecordLong(bg_noise_level_1_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(bg_noise_level_1_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

  int64_t bg_noise_level_2_roomA = 43;
  int64_t bg_noise_level_2_roomB = 25;

  storage.RecordLong(bg_noise_level_2_roomA,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                     opentelemetry::context::Context{});
  storage.RecordLong(bg_noise_level_2_roomB,
                     KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                     opentelemetry::context::Context{});

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          auto lastvalue_data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("Room.id")->second) == "Rack A")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                      bg_noise_level_2_roomA);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("Room.id")->second) == "Rack B")
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(lastvalue_data.value_),
                      bg_noise_level_2_roomB);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // RackA and RackB
#else
  EXPECT_TRUE(true);
#endif
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestLong,
                         WritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

TEST_P(WritableMetricStorageTestFixture, DoubleGaugeLastValueAggregation)
{
#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  AggregationTemporality temporality = GetParam();
  auto sdk_start_ts                  = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc    = {"name", "desc", "1unit", InstrumentType::kGauge,
                                        InstrumentValueType::kDouble};
  std::map<std::string, std::string> attributes_roomA = {{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attributes_roomB = {{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, default_attributes_processor,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  double bg_noise_level_1_roomA = 4.3;
  double bg_noise_level_1_roomB = 2.5;

  storage.RecordDouble(bg_noise_level_1_roomA,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                       opentelemetry::context::Context{});
  storage.RecordDouble(bg_noise_level_1_roomB,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                       opentelemetry::context::Context{});

  double bg_noise_level_2_roomA = 10.5;
  double bg_noise_level_2_roomB = 20.5;

  storage.RecordDouble(bg_noise_level_2_roomA,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomA),
                       opentelemetry::context::Context{});
  storage.RecordDouble(bg_noise_level_2_roomB,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes_roomB),
                       opentelemetry::context::Context{});

  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);

  // Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          auto lastvalue_data = opentelemetry::nostd::get<LastValuePointData>(data_attr.point_data);
          if (opentelemetry::nostd::get<std::string>(
                  data_attr.attributes.find("Room.id")->second) == "Rack A")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lastvalue_data.value_),
                             bg_noise_level_2_roomA);
            count_attributes++;
          }
          else if (opentelemetry::nostd::get<std::string>(
                       data_attr.attributes.find("Room.id")->second) == "Rack B")
          {
            EXPECT_DOUBLE_EQ(opentelemetry::nostd::get<double>(lastvalue_data.value_),
                             bg_noise_level_2_roomB);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, 2);  // RackA and RackB
#else
  EXPECT_TRUE(true);
#endif
}

INSTANTIATE_TEST_SUITE_P(WritableMetricStorageTestDouble,
                         WritableMetricStorageTestFixture,
                         ::testing::Values(AggregationTemporality::kCumulative,
                                           AggregationTemporality::kDelta));

// Tests for delta gauge: window isolation and stale-value suppression.
// These only apply to ABI v2+ where SyncMetricStorage is available.
#if OPENTELEMETRY_ABI_VERSION_NO >= 2

TEST(GaugeDeltaWindowTest, NoRecordWindowEmitsNothing)
{
  // record -> collect -> no record -> collect: second collection must emit nothing.
  auto sdk_start_ts = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc{"name", "desc", "1", InstrumentType::kGauge,
                                  InstrumentValueType::kLong};
  std::map<std::string, std::string> attrs{{"k", "v"}};

  std::shared_ptr<DefaultAttributesProcessor> proc{new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, proc,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  // Window 1: record then collect.
  storage.RecordLong(42, KeyValueIterableView<std::map<std::string, std::string>>(attrs),
                     opentelemetry::context::Context{});
  auto ts1            = std::chrono::system_clock::now();
  size_t count_w1     = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts, ts1,
                  [&](const MetricData &metric_data) {
                    count_w1 += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_EQ(count_w1, 1u);

  // Window 2: no recording, collect must emit nothing.
  auto ts2        = std::chrono::system_clock::now();
  size_t count_w2 = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts, ts2,
                  [&](const MetricData &metric_data) {
                    count_w2 += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_EQ(count_w2, 0u);
}

TEST(GaugeDeltaWindowTest, StaleAttributeNotReemitted)
{
  // record A+B -> collect -> record A only -> collect: second collection must not re-emit B.
  auto sdk_start_ts = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc{"name", "desc", "1", InstrumentType::kGauge,
                                  InstrumentValueType::kLong};
  std::map<std::string, std::string> attrs_a{{"Room.id", "Rack A"}};
  std::map<std::string, std::string> attrs_b{{"Room.id", "Rack B"}};

  std::shared_ptr<DefaultAttributesProcessor> proc{new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, proc,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  // Window 1: record both rooms, collect.
  storage.RecordLong(10, KeyValueIterableView<std::map<std::string, std::string>>(attrs_a),
                     opentelemetry::context::Context{});
  storage.RecordLong(20, KeyValueIterableView<std::map<std::string, std::string>>(attrs_b),
                     opentelemetry::context::Context{});
  auto ts1        = std::chrono::system_clock::now();
  size_t count_w1 = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts, ts1,
                  [&](const MetricData &metric_data) {
                    count_w1 += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_EQ(count_w1, 2u);

  // Window 2: record only Rack A; Rack B must not appear in the output.
  storage.RecordLong(43, KeyValueIterableView<std::map<std::string, std::string>>(attrs_a),
                     opentelemetry::context::Context{});
  auto ts2          = std::chrono::system_clock::now();
  size_t count_w2   = 0;
  int64_t val_rack_a = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, ts2, [&](const MetricData &metric_data) {
        for (const auto &pt : metric_data.point_data_attr_)
        {
          auto lv = opentelemetry::nostd::get<LastValuePointData>(pt.point_data);
          if (opentelemetry::nostd::get<std::string>(pt.attributes.find("Room.id")->second) ==
              "Rack A")
          {
            val_rack_a = opentelemetry::nostd::get<int64_t>(lv.value_);
          }
          count_w2++;
        }
        return true;
      });
  // Only Rack A should be emitted; Rack B value from W1 must not be re-exported.
  EXPECT_EQ(count_w2, 1u);
  EXPECT_EQ(val_rack_a, 43);
}

TEST(GaugeDeltaWindowTest, RecordAfterEmptyWindowEmitsNewValue)
{
  // record -> collect -> no record -> collect -> record -> collect: third window emits new value.
  auto sdk_start_ts = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc{"name", "desc", "1", InstrumentType::kGauge,
                                  InstrumentValueType::kLong};
  std::map<std::string, std::string> attrs{{"k", "v"}};

  std::shared_ptr<DefaultAttributesProcessor> proc{new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kLastValue, proc,
#  ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#  endif
      nullptr);

  std::shared_ptr<CollectorHandle> collector(
      new MockCollectorHandle(AggregationTemporality::kDelta));
  std::vector<std::shared_ptr<CollectorHandle>> collectors{collector};

  // Window 1.
  storage.RecordLong(10, KeyValueIterableView<std::map<std::string, std::string>>(attrs),
                     opentelemetry::context::Context{});
  auto ts1 = std::chrono::system_clock::now();
  storage.Collect(collector.get(), collectors, sdk_start_ts, ts1,
                  [&](const MetricData &) { return true; });

  // Window 2: empty.
  auto ts2        = std::chrono::system_clock::now();
  size_t count_w2 = 0;
  storage.Collect(collector.get(), collectors, sdk_start_ts, ts2,
                  [&](const MetricData &metric_data) {
                    count_w2 += metric_data.point_data_attr_.size();
                    return true;
                  });
  EXPECT_EQ(count_w2, 0u);

  // Window 3: new recording must be exported.
  storage.RecordLong(99, KeyValueIterableView<std::map<std::string, std::string>>(attrs),
                     opentelemetry::context::Context{});
  auto ts3          = std::chrono::system_clock::now();
  size_t count_w3   = 0;
  int64_t val_w3    = 0;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, ts3, [&](const MetricData &metric_data) {
        for (const auto &pt : metric_data.point_data_attr_)
        {
          val_w3 = opentelemetry::nostd::get<int64_t>(
              opentelemetry::nostd::get<LastValuePointData>(pt.point_data).value_);
          count_w3++;
        }
        return true;
      });
  EXPECT_EQ(count_w3, 1u);
  EXPECT_EQ(val_w3, 99);
}

#endif  // OPENTELEMETRY_ABI_VERSION_NO >= 2
