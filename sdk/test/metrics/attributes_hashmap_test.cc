// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/aggregation/drop_aggregation.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include <functional>

using namespace opentelemetry::sdk::metrics;
namespace nostd = opentelemetry::nostd;

TEST(AttributesHashMap, BasicTests)
{

  // Empty map
  AttributesHashMap hash_map;
  EXPECT_EQ(hash_map.Size(), 0);
  MetricAttributes m1 = {{"k1", "v1"}};
  EXPECT_EQ(hash_map.Get(m1), nullptr);
  EXPECT_EQ(hash_map.Has(m1), false);

  // Set
  std::unique_ptr<Aggregation> aggregation1(
      new DropAggregation());  //  = std::unique_ptr<Aggregation>(new DropAggregation);
  hash_map.Set(m1, std::move(aggregation1));
  EXPECT_NO_THROW(hash_map.Get(m1)->Aggregate(1l));
  EXPECT_EQ(hash_map.Size(), 1);
  EXPECT_EQ(hash_map.Has(m1), true);

  // Set same key again
  auto aggregation2 = std::unique_ptr<Aggregation>(new DropAggregation());
  hash_map.Set(m1, std::move(aggregation2));
  EXPECT_NO_THROW(hash_map.Get(m1)->Aggregate(1l));
  EXPECT_EQ(hash_map.Size(), 1);
  EXPECT_EQ(hash_map.Has(m1), true);

  // Set more enteria
  auto aggregation3   = std::unique_ptr<Aggregation>(new DropAggregation());
  MetricAttributes m3 = {{"k1", "v1"}, {"k2", "v2"}};
  hash_map.Set(m3, std::move(aggregation3));
  EXPECT_EQ(hash_map.Has(m1), true);
  EXPECT_EQ(hash_map.Has(m3), true);
  EXPECT_NO_THROW(hash_map.Get(m3)->Aggregate(1l));
  EXPECT_EQ(hash_map.Size(), 2);

  // GetOrSetDefault
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation =
      []() -> std::unique_ptr<Aggregation> {
    return std::unique_ptr<Aggregation>(new DropAggregation);
  };
  MetricAttributes m4 = {{"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}};
  EXPECT_NO_THROW(hash_map.GetOrSetDefault(m4, create_default_aggregation)->Aggregate(1l));
  EXPECT_EQ(hash_map.Size(), 3);

  // Set attributes with different order - shouldn't create a new entry.
  MetricAttributes m5 = {{"k2", "v2"}, {"k1", "v1"}};
  EXPECT_EQ(hash_map.Has(m5), true);

  // GetAllEnteries
  size_t count = 0;
  hash_map.GetAllEnteries([&count](const MetricAttributes &attributes, Aggregation &aggregation) {
    count++;
    return true;
  });
  EXPECT_EQ(count, hash_map.Size());
}

#endif