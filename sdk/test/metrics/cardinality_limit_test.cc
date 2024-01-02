// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "common.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
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
  // get the overflow metric point
  auto agg = hash_map.GetOrSetDefault(
      FilteredOrderedAttributeMap({{kAttributesLimitOverflowKey, kAttributesLimitOverflowValue}}),
      aggregation_callback, kOverflowAttributesHash);
  EXPECT_NE(agg, nullptr);
  auto sum_agg = static_cast<LongSumAggregation *>(agg);
  EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg->ToPoint()).value_),
            record_value * 6);  // 1 from previous 10, 5 from current 5.
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
                            ExemplarReservoir::GetNoExemplarReservoir(), nullptr, attributes_limit);

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
