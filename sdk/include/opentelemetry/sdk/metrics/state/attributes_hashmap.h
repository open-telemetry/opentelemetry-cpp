// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/version.h"

#include <functional>
#include <memory>
#include <unordered_map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
using opentelemetry::sdk::common::OrderedAttributeMap;

class AttributeHashGenerator
{
public:
  size_t operator()(const MetricAttributes &attributes) const
  {
    return opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
  }
};

class AttributesHashMap
{
public:
  Aggregation *Get(const MetricAttributes &attributes) const
  {
    auto it = hash_map_.find(attributes);
    if (it != hash_map_.end())
    {
      return it->second.get();
    }
    return nullptr;
  }

  /**
   * @return check if key is present in hash
   *
   */
  bool Has(const MetricAttributes &attributes) const
  {
    return (hash_map_.find(attributes) == hash_map_.end()) ? false : true;
  }

  /**
   * @return the pointer to value for given key if present.
   * If not present, it uses the provided callback to generate
   * value and store in the hash
   */
  Aggregation *GetOrSetDefault(const MetricAttributes &attributes,
                               std::function<std::unique_ptr<Aggregation>()> aggregation_callback)
  {
    auto it = hash_map_.find(attributes);
    if (it != hash_map_.end())
    {
      return it->second.get();
    }

    hash_map_[attributes] = aggregation_callback();
    return hash_map_[attributes].get();
  }

  /**
   * Set the value for given key, overwriting the value if already present
   */
  void Set(const MetricAttributes &attributes, std::unique_ptr<Aggregation> value)
  {
    hash_map_[attributes] = std::move(value);
  }

  /**
   * Iterate the hash to yield key and value stored in hash.
   */
  bool GetAllEnteries(
      nostd::function_ref<bool(const MetricAttributes &, Aggregation &)> callback) const
  {
    for (auto &kv : hash_map_)
    {
      if (!callback(kv.first, *(kv.second.get())))
      {
        return false;  // callback is not prepared to consume data
      }
    }
    return true;
  }

  /**
   * Return the size of hash.
   */
  size_t Size() { return hash_map_.size(); }

private:
  std::unordered_map<MetricAttributes, std::unique_ptr<Aggregation>, AttributeHashGenerator>
      hash_map_;
};
}  // namespace metrics

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
