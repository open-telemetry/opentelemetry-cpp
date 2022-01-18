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
/** Basic aggregator which calculates a Sum from individual measurements. */
template <class T>
class SumAggregator : public Aggregator<SumAccumulation<T>>
{
public:
  SumAggregator(InstrumentDescriptor &instrument_descriptor) {}
  SumAccumulation<T> CreateAccumulation() noexcept override { return SumAccumulation<T>(); }

  /** Returns the result of the merge of the given accumulations.*/

  SumAccumulation<T> Merge(SumAccumulation<T> &prev, SumAccumulation<T> &current) noexcept override
  {
    return SumAccumulation<T>(prev.ToPointData().value_ + current.ToPointData().value_);
  }

  /** Returns a new delta aggregation by comparing two cumulative measurements.*/
  SumAccumulation<T> diff(SumAccumulation<T> &prev, SumAccumulation<T> &current) noexcept override
  {
    return SumAccumulation<T>(prev.ToPointData().value_ - current.ToPointData().value_);
  }

  SingularMetricsData<T> ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<SumAccumulation<T>>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    SingularMetricsData<T> metrics_data = {resource, instrumentation_library,
                                           instrument_descriptor};
    for (auto &acc_by_attr : accumulation_by_attributes)
    {
      opentelemetry::sdk::metrics::BasePointData base_point_data = {start_epoch_ns, end_epoch_ns,
                                                                    acc_by_attr.attributes};
      auto point_data = acc_by_attr.accumulation.ToPointData();
      metrics_data.point_data_list.push_back(std::make_pair(base_point_data, point_data));
    }
    return metrics_data;
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif