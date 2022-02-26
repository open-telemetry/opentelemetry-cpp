// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#  include "opentelemetry/sdk/metrics/view/view.h"
#  include "opentelemetry/sdk/resource/resource.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class SyncMetricStorage : public MetricStorage, public WritableMetricStorage
{

public:
  SyncMetricStorage(InstrumentDescriptor instrument_descriptor,
                    const AggregationType aggregation_type,
                    const AttributesProcessor *attributes_processor)
      : instrument_descriptor_(instrument_descriptor),
        aggregation_type_{aggregation_type},
        attributes_hashmap_(new AttributesHashMap()),
        attributes_processor_{attributes_processor}
  {
    create_default_aggregation_ = [&]() -> std::unique_ptr<Aggregation> {
      return std::move(
          DefaultAggregation::CreateAggregation(aggregation_type_, instrument_descriptor_));
    };
  }

  void RecordLong(long value) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }
    attributes_hashmap_->GetOrSetDefault({}, create_default_aggregation_)->Aggregate(value);
  }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kLong)
    {
      return;
    }

    auto attr = attributes_processor_->process(attributes);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
  }

  void RecordDouble(double value) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }

    attributes_hashmap_->GetOrSetDefault({}, create_default_aggregation_)->Aggregate(value);
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    if (instrument_descriptor_.value_type_ != InstrumentValueType::kDouble)
    {
      return;
    }

    auto attr = attributes_processor_->process(attributes);
    attributes_hashmap_->GetOrSetDefault(attr, create_default_aggregation_)->Aggregate(value);
  }

  bool Collect(
      MetricCollector *collector,
      nostd::span<MetricCollector *> collectors,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::resource::Resource *resource,
      nostd::function_ref<bool(MetricData &)> callback) noexcept override
  {
    MetricData metric_data;
    if (callback(metric_data))
    {
      return true;
    }
    return false;
  }

private:
  InstrumentDescriptor instrument_descriptor_;
  AggregationType aggregation_type_;
  std::unique_ptr<AttributesHashMap> attributes_hashmap_;
  const AttributesProcessor *attributes_processor_;
  std::function<std::unique_ptr<Aggregation>()> create_default_aggregation_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif