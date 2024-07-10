// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#endif
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"

#include <gtest/gtest.h>
#include <functional>

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;
namespace nostd = opentelemetry::nostd;

TEST(CardinalityLimit, AttributesHashMapBasicTests)
{
  AttributesHashMap hash_map(10);
  std::function<std::unique_ptr<Aggregation>()> aggregation_callback =
      []() -> std::unique_ptr<Aggregation> {
    return std::unique_ptr<Aggregation>(new LongSumAggregation(true));
  };
  // add 10 unique metric points. 9 should be added to hashmap, 10th should be overflow.
  long record_value = 100;
  for (auto i = 0; i < 10; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    static_cast<LongSumAggregation *>(
        hash_map.GetOrSetDefault(attributes, aggregation_callback, hash))
        ->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10);
  // add 5 unique metric points above limit, they all should get consolidated as single
  // overflowmetric point.
  for (auto i = 10; i < 15; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    static_cast<LongSumAggregation *>(
        hash_map.GetOrSetDefault(attributes, aggregation_callback, hash))
        ->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10);  // only one more metric point should be added as overflow.
  // record 5 more measurements to already existing (and not-overflow) metric points. They
  // should get aggregated to these existing metric points.
  for (auto i = 0; i < 5; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    static_cast<LongSumAggregation *>(
        hash_map.GetOrSetDefault(attributes, aggregation_callback, hash))
        ->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10); // no new metric point added

  // get the overflow metric point
  auto agg = hash_map.GetOrSetDefault(
      FilteredOrderedAttributeMap({{kAttributesLimitOverflowKey, kAttributesLimitOverflowValue}}),
      aggregation_callback, kOverflowAttributesHash);
  EXPECT_NE(agg, nullptr);
  auto sum_agg = static_cast<LongSumAggregation *>(agg);
  EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg->ToPoint()).value_),
            record_value * 6);  // 1 from previous 10, 5 from current 5.
  // get remaining metric points
  for (auto i = 0 ; i < 9 ; i ++) {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    auto agg = hash_map.GetOrSetDefault(
      FilteredOrderedAttributeMap({{kAttributesLimitOverflowKey, kAttributesLimitOverflowValue}}),
      aggregation_callback, hash);
    EXPECT_NE(agg, nullptr);
    auto sum_agg = static_cast<LongSumAggregation *>(agg);
    if (i < 5) {
      EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg->ToPoint()).value_),
            record_value * 2);  // 1 from first recording, 1 from third recording
    } else {
      EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg->ToPoint()).value_),
            record_value);  // 1 from first recording
    }
  }
}

TEST(WritableMetricStorageTestFixture, SyncStorageCardinalityLimit)
{
  const auto kCardinalityLimit        = 10;
  AggregationTemporality temporality  = AggregationTemporality::kCumulative;
  auto sdk_start_ts                   = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc     = {"name", "desc", "1unit", InstrumentType::kCounter,
                                         InstrumentValueType::kLong};

  std::unique_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  opentelemetry::sdk::metrics::SyncMetricStorage storage(
      instr_desc, AggregationType::kSum, default_attributes_processor.get(),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
      ExemplarFilterType::kAlwaysOff, ExemplarReservoir::GetNoExemplarReservoir(),
#endif
      nullptr, kCardinalityLimit);

  int64_t first_recorded_value = 10;
  for (auto i = 0; i < 10; i++)
  {
    std::map<std::string, std::string> attributes = {{"key", std::to_string(i)}};
    storage.RecordLong(first_recorded_value,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                       opentelemetry::context::Context{});
  }

  // record again
  int64_t second_recorded_value = 30;
  for (auto i = 0; i < 10; i++)
  {
    std::map<std::string, std::string> attributes = {{"key", std::to_string(i)}};
    storage.RecordLong(second_recorded_value,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                       opentelemetry::context::Context{});
  }
  // record 5 more with new attributes to go to overflow metrics
  int64_t third_recorded_value = 40;
  for (auto i = 11; i < 16; i++)
  {
    std::map<std::string, std::string> attributes = {{"key", std::to_string(i)}};
    storage.RecordLong(third_recorded_value,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                       opentelemetry::context::Context{});
  }

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
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          if (data_attr.attributes.find("key") != data_attr.attributes.end())
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_),
                      first_recorded_value + second_recorded_value);
            count_attributes++;
          }
          else if (data_attr.attributes.find(kAttributesLimitOverflowKey) !=
                   data_attr.attributes.end())
          {
            EXPECT_EQ(opentelemetry::nostd::get<int64_t>(data.value_),
                      first_recorded_value + second_recorded_value + 5 * third_recorded_value);
            count_attributes++;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, kCardinalityLimit);  // Max cardinality limit
}

class WritableMetricStorageCardinalityLimitTestFixture
    : public ::testing::TestWithParam<AggregationTemporality>
{};

TEST_P(WritableMetricStorageCardinalityLimitTestFixture, LongCounterSumAggregation)
{
  auto sdk_start_ts               = std::chrono::system_clock::now();
  const size_t attributes_limit   = 10;
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  std::unique_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  SyncMetricStorage storage(instr_desc, AggregationType::kSum, default_attributes_processor.get(),
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
                            ExemplarFilterType::kAlwaysOff,
                            ExemplarReservoir::GetNoExemplarReservoir(),
#endif
                            nullptr, attributes_limit);

  long record_value = 100;
  // add 9 unique metric points, and 6 more above limit.
  for (auto i = 0; i < 15; i++)
  {
    std::map<std::string, std::string> attributes = {{"key", std::to_string(i)}};
    storage.RecordLong(record_value,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                       opentelemetry::context::Context{});
  }
  AggregationTemporality temporality = GetParam();
  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);
  //... Some computation here
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  bool overflow_present   = false;
  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
          count_attributes++;
          if (data_attr.attributes.begin()->first == kAttributesLimitOverflowKey)
          {
            EXPECT_EQ(nostd::get<int64_t>(data.value_), record_value * 6);
            overflow_present = true;
          }
        }
        return true;
      });
  EXPECT_EQ(count_attributes, attributes_limit);
  EXPECT_EQ(overflow_present, true);
}
INSTANTIATE_TEST_SUITE_P(All,
                         WritableMetricStorageCardinalityLimitTestFixture,
                         ::testing::Values(AggregationTemporality::kDelta));
