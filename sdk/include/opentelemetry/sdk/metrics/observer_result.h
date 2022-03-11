// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/metrics/observer_result.h"
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"

#  include <map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
template <class T>
class ObserverResult final : public opentelemetry::metrics::ObserverResult<T>
{
public:
  ObserverResult(const AttributesProcessor *attributes_processor)
      : attributes_processor_(attributes_processor)
  {}

  void Observe(T value) noexcept override { data_.insert({{}, value}); }

  void Observe(T value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    auto attr = attributes_processor_->process(attributes);
    data_.insert({attr, value});
  }

  const std::unordered_map<MetricAttributes, T, AttributeHashGenerator> &GetMeasurements()
  {
    return data_;
  }

private:
  std::unordered_map<MetricAttributes, T, AttributeHashGenerator> data_;

  const AttributesProcessor *attributes_processor_;
};
}  // namespace metrics
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
