// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/aggregator/accumulation.h"
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/** Aggregators to drop the instruments. */
template <class T>
class DropAggregator : public Aggregator<DropAccumulation<T>>
{
public:
  DropAccumulation<T> CreateAccumulation() noexcept override { return DropAccumulation<T>(); }

  /** Returns the result of the merge of the given accumulations.*/
  DropAccumulation<T> Merge(DropAccumulation<T> &prev,
                            DropAccumulation<T> &current) noexcept override
  {
    return DropAccumulation<T>();
  }

  /** Returns a new delta aggregation by comparing two cumulative measurements.*/
  DropAccumulation<T> diff(DropAccumulation<T> &prev,
                           DropAccumulation<T> &current) noexcept override
  {
    return DropAccumulation<T>(prev.ToPointData().value_ - current.ToPointData().value_);
  }

  DropMetricData<T> ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<DropAccumulation<T>>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    DropMetricData<T> metrics_data = {resource, instrumentation_library, instrument_descriptor};
    return metrics_data;
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif