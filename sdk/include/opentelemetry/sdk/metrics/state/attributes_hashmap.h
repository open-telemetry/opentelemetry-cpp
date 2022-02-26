// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/nostd/function_ref.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"

#  include <functional>
#  include <memory>
#  include <mutex>
#  include <unordered_map>

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
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
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
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
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
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    auto it = hash_map_.find(attributes);
    if (it != hash_map_.end())
    {
      return it->second.get();
    }

    hash_map_[attributes] = std::move(aggregation_callback());
    return hash_map_[attributes].get();
  }

  /**
   * Set the value for given key, overwriting the value if already present
   */
  void Set(const MetricAttributes &attributes, std::unique_ptr<Aggregation> value)
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    hash_map_[attributes] = std::move(value);
  }

  /**
   * Iterate the hash to yield key and value stored in hash.
   */
  bool GetAllEnteries(
      nostd::function_ref<bool(const MetricAttributes &, Aggregation &)> callback) const
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
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
  size_t Size()
  {
    std::lock_guard<opentelemetry::common::SpinLockMutex> guard(GetLock());
    return hash_map_.size();
  }

private:
  std::unordered_map<MetricAttributes, std::unique_ptr<Aggregation>, AttributeHashGenerator>
      hash_map_;

  static opentelemetry::common::SpinLockMutex &GetLock() noexcept
  {
    static opentelemetry::common::SpinLockMutex lock;
    return lock;
  }
};
}  // namespace metrics

}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
