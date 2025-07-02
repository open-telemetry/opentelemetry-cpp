// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
#include <stdint.h>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "common.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/sdk/metrics/state/metric_collector.h"
#include "opentelemetry/sdk/metrics/state/sync_metric_storage.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/sdk/metrics/view/view.h"

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/filter_type.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"
#endif

using namespace opentelemetry::sdk::metrics;
using namespace opentelemetry::common;
namespace nostd = opentelemetry::nostd;

TEST(CardinalityLimit, ViewCardinalityLimitConfiguration)
{
  // Test View without cardinality limit
  View view_no_limit("test_view_no_limit");
  EXPECT_FALSE(view_no_limit.HasAggregationCardinalityLimit());
  EXPECT_EQ(view_no_limit.GetAggregationCardinalityLimit(), 0);

  // Test View with cardinality limit
  View view_with_limit("test_view_with_limit", "", "", AggregationType::kDefault, nullptr,
                       std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>(
                           new opentelemetry::sdk::metrics::DefaultAttributesProcessor()),
                       500);
  EXPECT_TRUE(view_with_limit.HasAggregationCardinalityLimit());
  EXPECT_EQ(view_with_limit.GetAggregationCardinalityLimit(), 500);
}

TEST(CardinalityLimit, AttributesHashMapBasicTests)
{
  AttributesHashMap hash_map(10);
  std::function<std::unique_ptr<Aggregation>()> aggregation_callback =
      []() -> std::unique_ptr<Aggregation> {
    return std::unique_ptr<Aggregation>(new LongSumAggregation(true));
  };
  // add 10 unique metric points. 9 should be added to hashmap, 10th should be overflow.
  int64_t record_value = 100;
  for (auto i = 0; i < 10; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    static_cast<LongSumAggregation *>(hash_map.GetOrSetDefault(attributes, aggregation_callback))
        ->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10);
  // add 5 unique metric points above limit, they all should get consolidated as single
  // overflowmetric point.
  for (auto i = 10; i < 15; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    static_cast<LongSumAggregation *>(hash_map.GetOrSetDefault(attributes, aggregation_callback))
        ->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10);  // only one more metric point should be added as overflow.
  // record 5 more measurements to already existing (and not-overflow) metric points. They
  // should get aggregated to these existing metric points.
  for (auto i = 0; i < 5; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    static_cast<LongSumAggregation *>(hash_map.GetOrSetDefault(attributes, aggregation_callback))
        ->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10);  // no new metric point added

  // get the overflow metric point
  auto agg1 = hash_map.GetOrSetDefault(kOverflowAttributes, aggregation_callback);
  EXPECT_NE(agg1, nullptr);
  auto sum_agg1 = static_cast<LongSumAggregation *>(agg1);
  EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg1->ToPoint()).value_),
            record_value * 6);  // 1 from previous 10, 5 from current 5.
  // get remaining metric points
  for (auto i = 0; i < 9; i++)
  {
    FilteredOrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto agg2 = hash_map.GetOrSetDefault(attributes, aggregation_callback);
    EXPECT_NE(agg2, nullptr);
    auto sum_agg2 = static_cast<LongSumAggregation *>(agg2);
    if (i < 5)
    {
      EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg2->ToPoint()).value_),
                record_value * 2);  // 1 from first recording, 1 from third recording
    }
    else
    {
      EXPECT_EQ(nostd::get<int64_t>(nostd::get<SumPointData>(sum_agg2->ToPoint()).value_),
                record_value);  // 1 from first recording
    }
  }
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
  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};
  SyncMetricStorage storage(instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
                            ExemplarFilterType::kAlwaysOff,
                            ExemplarReservoir::GetNoExemplarReservoir(),
#endif
                            nullptr, attributes_limit);

  int64_t record_value = 100;
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

TEST(CardinalityLimit, SyncMetricStorageWithViewCardinalityLimit)
{
  auto sdk_start_ts               = std::chrono::system_clock::now();
  InstrumentDescriptor instr_desc = {"name", "desc", "1unit", InstrumentType::kCounter,
                                     InstrumentValueType::kLong};
  std::shared_ptr<DefaultAttributesProcessor> default_attributes_processor{
      new DefaultAttributesProcessor{}};

  // Create a view with a cardinality limit of 5
  View view_with_limit("test_view", "", "", AggregationType::kSum, nullptr,
                       std::unique_ptr<opentelemetry::sdk::metrics::AttributesProcessor>(
                           new opentelemetry::sdk::metrics::DefaultAttributesProcessor()),
                       5);

  // Test that the view has the cardinality limit
  EXPECT_TRUE(view_with_limit.HasAggregationCardinalityLimit());
  EXPECT_EQ(view_with_limit.GetAggregationCardinalityLimit(), 5);

  // Create SyncMetricStorage using the cardinality limit from the view
  size_t cardinality_limit = view_with_limit.HasAggregationCardinalityLimit()
                                 ? view_with_limit.GetAggregationCardinalityLimit()
                                 : kAggregationCardinalityLimit;
  SyncMetricStorage storage(instr_desc, AggregationType::kSum, default_attributes_processor,
#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW
                            ExemplarFilterType::kAlwaysOff,
                            ExemplarReservoir::GetNoExemplarReservoir(),
#endif
                            nullptr, cardinality_limit);

  int64_t record_value = 100;
  // Add 5 unique metric points (should all fit within limit)
  for (auto i = 0; i < 5; i++)
  {
    std::map<std::string, std::string> attributes = {{"key", std::to_string(i)}};
    storage.RecordLong(record_value,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                       opentelemetry::context::Context{});
  }

  // Add 3 more unique metric points (should trigger overflow behavior)
  for (auto i = 5; i < 8; i++)
  {
    std::map<std::string, std::string> attributes = {{"key", std::to_string(i)}};
    storage.RecordLong(record_value,
                       KeyValueIterableView<std::map<std::string, std::string>>(attributes),
                       opentelemetry::context::Context{});
  }

  AggregationTemporality temporality = AggregationTemporality::kDelta;
  std::shared_ptr<CollectorHandle> collector(new MockCollectorHandle(temporality));
  std::vector<std::shared_ptr<CollectorHandle>> collectors;
  collectors.push_back(collector);
  auto collection_ts      = std::chrono::system_clock::now();
  size_t count_attributes = 0;
  bool overflow_present   = false;

  storage.Collect(
      collector.get(), collectors, sdk_start_ts, collection_ts, [&](const MetricData &metric_data) {
        for (const auto &data_attr : metric_data.point_data_attr_)
        {
          count_attributes++;
          if (data_attr.attributes.begin()->first == kAttributesLimitOverflowKey)
          {
            // The overflow attribute should contain the aggregated values from the 3 excess metrics
            const auto &data = opentelemetry::nostd::get<SumPointData>(data_attr.point_data);
            EXPECT_EQ(nostd::get<int64_t>(data.value_), record_value * 3);
            overflow_present = true;
          }
        }
        return true;
      });

  // We should have exactly 5 attributes (the cardinality limit)
  EXPECT_EQ(count_attributes, 5);
  // And there should be an overflow attribute
  EXPECT_TRUE(overflow_present);
}
