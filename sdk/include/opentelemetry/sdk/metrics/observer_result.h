// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#include "opentelemetry/sdk/metrics/state/measurement_attributes_map.h"
#include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
template <class T>
class ObserverResultT final : public opentelemetry::metrics::ObserverResultT<T>
{
public:
  explicit ObserverResultT(std::shared_ptr<AttributesProcessor> attributes_processor = nullptr)
      : attributes_processor_(attributes_processor)
  {}

  ~ObserverResultT() override = default;

  void Observe(T value) noexcept override
  {
    std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> empty;
    data_[empty] += value;
  }

  void Observe(T value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue> attr_map;
    attributes.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue val) noexcept {
          attr_map.emplace(key, val);
          return true;
        });
    data_[attr_map] += value;  // overwrites the previous value if present
  }

  const MeasurementAttributes<T> &GetMeasurements() { return data_; }

  std::shared_ptr<AttributesProcessor> GetAttributesProcessor() const noexcept
  {
    return attributes_processor_;
  }

private:
  MeasurementAttributes<T> data_;
  std::shared_ptr<AttributesProcessor> attributes_processor_;
};

}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
