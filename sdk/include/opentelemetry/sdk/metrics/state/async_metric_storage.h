// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/common/attributemap_hash.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/state/attributes_hashmap.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"
#  include "opentelemetry/sdk/metrics/view/attributes_processor.h"
#  include "opentelemetry/sdk/resource/resource.h"

#  include <memory>
#  include "opentelemetry/sdk/metrics/observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template <class T>
class AsyncMetricStorage : public MetricStorage
{
public:
  AsyncMetricStorage(InstrumentDescriptor instrument_descriptor,
                     const AggregationType aggregation_type,
                     void (*measurement_callback)(opentelemetry::metrics::ObserverResult<T> &),
                     const AttributesProcessor *attributes_processor)
      : instrument_descriptor_(instrument_descriptor),
        aggregation_type_{aggregation_type},
        measurement_collection_callback_{measurement_callback},
        attributes_processor_{attributes_processor},
        active_attributes_hashmap_(new AttributesHashMap())
  {}

  bool Collect(
      MetricCollector *collector,
      nostd::span<MetricCollector *> collectors,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::resource::Resource *resource,
      nostd::function_ref<bool(MetricData &)> metric_collection_callback) noexcept override
  {
    opentelemetry::sdk::metrics::ObserverResult<T> ob_res(attributes_processor_);

    // read the measurement using configured callback
    measurement_collection_callback_(ob_res);

    // process the read measurements - aggregate and store in hashmap
    for (auto &measurement : ob_res.GetMeasurements())
    {
      auto agg = DefaultAggregation::CreateAggregation(aggregation_type_, instrument_descriptor_);
      agg->Aggregate(measurement.second);
      active_attributes_hashmap_->Set(measurement.first, std::move(agg));
    }

    // TBD -> read aggregation from hashmap, and perform metric collection
    MetricData metric_data;
    if (metric_collection_callback(metric_data))
    {
      return true;
    }
    return false;
  }

private:
  InstrumentDescriptor instrument_descriptor_;
  AggregationType aggregation_type_;
  void (*measurement_collection_callback_)(opentelemetry::metrics::ObserverResult<T> &);
  const AttributesProcessor *attributes_processor_;
  std::unique_ptr<AttributesHashMap> active_attributes_hashmap_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif