// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/drop_aggregation.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"

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
  hash_map.Set(m1, std::move(aggregation1), 0);
  hash_map.Get(m1)->Aggregate(static_cast<int64_t>(1));
  EXPECT_EQ(hash_map.Size(), 1);
  EXPECT_EQ(hash_map.Has(m1), true);

  // Set same key again
  auto aggregation2 = std::unique_ptr<Aggregation>(new DropAggregation());
  hash_map.Set(m1, std::move(aggregation2), 0);
  hash_map.Get(m1)->Aggregate(static_cast<int64_t>(1));
  EXPECT_EQ(hash_map.Size(), 1);
  EXPECT_EQ(hash_map.Has(m1), true);

  // Set more enteria
  auto aggregation3   = std::unique_ptr<Aggregation>(new DropAggregation());
  MetricAttributes m3 = {{"k1", "v1"}, {"k2", "v2"}};
  hash_map.Set(m3, std::move(aggregation3), 0);
  EXPECT_EQ(hash_map.Has(m1), true);
  EXPECT_EQ(hash_map.Has(m3), true);
  hash_map.Get(m3)->Aggregate(static_cast<int64_t>(1));
  EXPECT_EQ(hash_map.Size(), 2);

  // GetOrSetDefault
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation =
      []() -> std::unique_ptr<Aggregation> {
    return std::unique_ptr<Aggregation>(new DropAggregation);
  };
  MetricAttributes m4 = {{"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}};
  hash_map.GetOrSetDefault(m4, create_default_aggregation, 0)
      ->Aggregate(static_cast<int64_t>(1));
  EXPECT_EQ(hash_map.Size(), 3);

  // Set attributes with different order - shouldn't create a new entry.
  MetricAttributes m5 = {{"k2", "v2"}, {"k1", "v1"}};
  EXPECT_EQ(hash_map.Has(m5), true);

  // GetAllEnteries
  size_t count = 0;
  hash_map.GetAllEnteries(
      [&count](const MetricAttributes & /* attributes */, Aggregation & /* aggregation */) {
        count++;
        return true;
      });
  EXPECT_EQ(count, hash_map.Size());
}

std::string make_unique_string(const char *str)
{
  return std::string(str);
}

TEST(AttributesHashMap, HashConsistencyAcrossStringTypes)
{
  const char *c_str                 = "teststring";
  std::string std_str               = "teststring";
  nostd::string_view nostd_str_view = "teststring";
#if __cplusplus >= 201703L
  std::string_view std_str_view = "teststring";
#endif

  size_t hash_c_str          = 0;
  size_t hash_std_str        = 0;
  size_t hash_nostd_str_view = 0;
#if __cplusplus >= 201703L
  size_t hash_std_str_view = 0;
#endif

  opentelemetry::sdk::common::GetHash(hash_c_str, c_str);
  opentelemetry::sdk::common::GetHash(hash_std_str, std_str);
  opentelemetry::sdk::common::GetHash(hash_nostd_str_view, nostd_str_view);
#if __cplusplus >= 201703L
  opentelemetry::sdk::common::GetHash(hash_std_str_view, std_str_view);
#endif

  EXPECT_EQ(hash_c_str, hash_std_str);
  EXPECT_EQ(hash_c_str, hash_nostd_str_view);
#if __cplusplus >= 201703L
  EXPECT_EQ(hash_c_str, hash_std_str_view);
#endif
}
