// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include <gtest/gtest.h>
#include "opentelemetry/sdk/metrics/aggregation/drop_aggregation.h"
#include "opentelemetry/sdk/metrics/instruments.h"

#include <functional>

using namespace opentelemetry::sdk::metrics;
namespace nostd = opentelemetry::nostd;

// Mock KeyValueIterable implementation
class MockKeyValueIterable : public opentelemetry::common::KeyValueIterable
{
public:
  MockKeyValueIterable(
      std::initializer_list<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>>
          init)
      : attributes_(init)
  {}

  bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback)
      const noexcept override
  {
    for (const auto &kv : attributes_)
    {
      if (!callback(kv.first, kv.second))
      {
        return false;
      }
    }
    return true;
  }

  size_t size() const noexcept override { return attributes_.size(); }

private:
  std::vector<std::pair<nostd::string_view, opentelemetry::common::AttributeValue>> attributes_;
};

TEST(AttributesHashMap, BasicTests)
{
  // Empty map
  AttributesHashMap hash_map;
  EXPECT_EQ(hash_map.Size(), 0);
  MetricAttributes m1 = {{"k1", "v1"}};
  auto hash           = opentelemetry::sdk::common::GetHashForAttributeMap(m1);

  EXPECT_EQ(hash_map.Get(hash), nullptr);
  EXPECT_EQ(hash_map.Has(hash), false);

  // Set
  std::unique_ptr<Aggregation> aggregation1(
      new DropAggregation());  //  = std::unique_ptr<Aggregation>(new DropAggregation);
  hash_map.Set(m1, std::move(aggregation1), hash);
  EXPECT_NO_THROW(hash_map.Get(hash)->Aggregate(static_cast<int64_t>(1)));
  EXPECT_EQ(hash_map.Size(), 1);
  EXPECT_EQ(hash_map.Has(hash), true);

  // Set same key again
  auto aggregation2 = std::unique_ptr<Aggregation>(new DropAggregation());
  hash_map.Set(m1, std::move(aggregation2), hash);
  EXPECT_NO_THROW(hash_map.Get(hash)->Aggregate(static_cast<int64_t>(1)));
  EXPECT_EQ(hash_map.Size(), 1);
  EXPECT_EQ(hash_map.Has(hash), true);

  // Set more enteria
  auto aggregation3   = std::unique_ptr<Aggregation>(new DropAggregation());
  MetricAttributes m3 = {{"k1", "v1"}, {"k2", "v2"}};
  auto hash3          = opentelemetry::sdk::common::GetHashForAttributeMap(m3);
  hash_map.Set(m3, std::move(aggregation3), hash3);
  EXPECT_EQ(hash_map.Has(hash), true);
  EXPECT_EQ(hash_map.Has(hash3), true);
  EXPECT_NO_THROW(hash_map.Get(hash3)->Aggregate(static_cast<int64_t>(1)));
  EXPECT_EQ(hash_map.Size(), 2);

  // GetOrSetDefault
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation =
      []() -> std::unique_ptr<Aggregation> {
    return std::unique_ptr<Aggregation>(new DropAggregation);
  };
  MetricAttributes m4 = {{"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}};
  auto hash4          = opentelemetry::sdk::common::GetHashForAttributeMap(m4);
  EXPECT_NO_THROW(hash_map.GetOrSetDefault(m4, create_default_aggregation, hash4)
                      ->Aggregate(static_cast<int64_t>(1)));
  EXPECT_EQ(hash_map.Size(), 3);

  // Set attributes with different order - shouldn't create a new entry.
  MetricAttributes m5 = {{"k2", "v2"}, {"k1", "v1"}};
  auto hash5          = opentelemetry::sdk::common::GetHashForAttributeMap(m5);
  EXPECT_EQ(hash_map.Has(hash5), true);

  // GetAllEnteries
  size_t count = 0;
  hash_map.GetAllEnteries(
      [&count](const MetricAttributes & /* attributes */, Aggregation & /* aggregation */) {
        count++;
        return true;
      });
  EXPECT_EQ(count, hash_map.Size());
}

TEST(AttributesHashMap, HashWithKeyValueIterable)
{
  // Create mock KeyValueIterable instances with the same content but different variables
  MockKeyValueIterable attributes1({{"k1", "v1"}, {"k2", "v2"}});
  MockKeyValueIterable attributes2({{"k1", "v1"}, {"k2", "v2"}});

  // Create a callback that accepts all keys
  auto is_key_present_callback = [](nostd::string_view /*key*/) {
    return true;  // Consider all keys
  };

  // Calculate hash
  size_t hash1 =
      opentelemetry::sdk::common::GetHashForAttributeMap(attributes1, is_key_present_callback);
  size_t hash2 =
      opentelemetry::sdk::common::GetHashForAttributeMap(attributes2, is_key_present_callback);

  // Expect the hashes to be the same because the content is the same
  EXPECT_EQ(hash1, hash2);

  // Create mock KeyValueIterable instance with different order
  MockKeyValueIterable attributes3({{"k2", "v2"}, {"k1", "v1"}});
  size_t hash3 =
      opentelemetry::sdk::common::GetHashForAttributeMap(attributes3, is_key_present_callback);

  // Expect the hash to be different because the order is different
  EXPECT_NE(hash1, hash3);
}
