// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/attributemap_hash.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
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
  AttributesHashMap(
      const AttributesProcessor *attributes_processor = new DefaultAttributesProcessor())
      : attributes_processor_{attributes_processor}
  {}

  Aggregation *Get(const opentelemetry::common::KeyValueIterable &attributes) const
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(
        attributes,
        [this](nostd::string_view key) { return attributes_processor_->isPresent(key); });

    auto it = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      return it->second.second.get();
    }
    return nullptr;
  }

  Aggregation *Get(const MetricAttributes &attributes) const
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    auto it   = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      return it->second.second.get();
    }
    return nullptr;
  }

  /**
   * @return check if key is present in hash
   *
   */
  bool Has(const opentelemetry::common::KeyValueIterable &attributes) const
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(
        attributes,
        [this](nostd::string_view key) { return attributes_processor_->isPresent(key); });

    return (hash_map_.find(hash) == hash_map_.end()) ? false : true;
  }

  bool Has(const MetricAttributes &attributes) const
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);

    return (hash_map_.find(hash) == hash_map_.end()) ? false : true;
  }

  /**
   * @return the pointer to value for given key if present.
   * If not present, it uses the provided callback to generate
   * value and store in the hash
   */
  Aggregation *GetOrSetDefault(const opentelemetry::common::KeyValueIterable &attributes,
                               std::function<std::unique_ptr<Aggregation>()> aggregation_callback)
  {

    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(
        attributes,
        [this](nostd::string_view key) { return attributes_processor_->isPresent(key); });

    auto it = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      return it->second.second.get();
    }

    MetricAttributes attr{attributes};

    hash_map_[hash] = {attr, aggregation_callback()};
    return hash_map_[hash].second.get();
  }

  Aggregation *GetOrSetDefault(std::function<std::unique_ptr<Aggregation>()> aggregation_callback)
  {
    static size_t hash = opentelemetry::sdk::common::GetHash("");
    auto it            = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      return it->second.second.get();
    }
    MetricAttributes attr{};
    hash_map_[hash] = {attr, aggregation_callback()};
    return hash_map_[hash].second.get();
  }

  Aggregation *GetOrSetDefault(const MetricAttributes &attributes,
                               std::function<std::unique_ptr<Aggregation>()> aggregation_callback)
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);

    auto it = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      return it->second.second.get();
    }

    MetricAttributes attr{attributes};

    hash_map_[hash] = {attr, aggregation_callback()};
    return hash_map_[hash].second.get();
  }

  /**
   * Set the value for given key, overwriting the value if already present
   */
  void Set(const opentelemetry::common::KeyValueIterable &attributes,
           std::unique_ptr<Aggregation> aggr)
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(
        attributes,
        [this](nostd::string_view key) { return attributes_processor_->isPresent(key); });
    auto it = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      it->second.second = std::move(aggr);
    }
    else
    {
      MetricAttributes attr{attributes};
      hash_map_[hash] = {attr, std::move(aggr)};
    }
  }

  void Set(const MetricAttributes &attributes, std::unique_ptr<Aggregation> aggr)
  {
    auto hash = opentelemetry::sdk::common::GetHashForAttributeMap(attributes);
    auto it   = hash_map_.find(hash);
    if (it != hash_map_.end())
    {
      it->second.second = std::move(aggr);
    }
    else
    {
      MetricAttributes attr{attributes};
      hash_map_[hash] = {attr, std::move(aggr)};
    }
  }

  /**
   * Iterate the hash to yield key and value stored in hash.
   */
  bool GetAllEnteries(
      nostd::function_ref<bool(const MetricAttributes &, Aggregation &)> callback) const
  {
    for (auto &kv : hash_map_)
    {
      if (!callback(kv.second.first, *(kv.second.second.get())))
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

  const AttributesProcessor *GetAttributesProcessor() { return attributes_processor_; }

private:
  std::unordered_map<size_t, std::pair<MetricAttributes, std::unique_ptr<Aggregation>>> hash_map_;
  const AttributesProcessor *attributes_processor_;
};
}  // namespace metrics

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
