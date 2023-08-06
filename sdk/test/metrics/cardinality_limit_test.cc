// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/sdk/metrics/aggregation/sum_aggregation.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"

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
  // add 10 unique metric points.
  long record_value = 100;
  for (auto i = 0; i < 10; i++)
  {
    OrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto hash                      = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    hash_map.GetOrSetDefault(attributes, aggregation_callback, hash)->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 10);
  // add 5 unique metric points above limit
  for (auto i = 10; i < 15; i++)
  {
    OrderedAttributeMap attributes = {{"key", std::to_string(i)}};
    auto hash                      = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    hash_map.GetOrSetDefault(attributes, aggregation_callback, hash)->Aggregate(record_value);
  }
  EXPECT_EQ(hash_map.Size(), 11);  // only one more metric point should be added as overflow.
  // get the overflow metric point
  auto agg = hash_map.GetOrSetDefault(
      OrderedAttributeMap({{kAttributesLimitOverflowKey, kAttributesLimitOverflowValue}}),
      aggregation_callback, kOverflowAttributesHash);
  EXPECT_NE(agg, nullptr);
  auto sum_agg = reinterpret_cast<LongSumAggregation *>(agg);
  EXPECT_EQ(nostd::get<long>(nostd::get<SumPointData>(sum_agg->ToPoint()).value_),
            record_value * 5);
}