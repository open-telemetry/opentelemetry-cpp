// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/sdk/metrics/aggregator/accumulation.h"
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"

#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/** Basic aggregator which calculates a last value from individual measurements. */
class LongLastValueAggregator : public Aggregator
{
public:
  std::unique_ptr<Accumulation> CreateAccumulation() noexcept override
  {
    return std::move(std::unique_ptr<Accumulation>(new LongLastValueAccumulation()));
  }

  /** Returns the result of the merge of the given accumulations.*/

  std::unique_ptr<Accumulation> Merge(Accumulation &prev, Accumulation &current) noexcept override
  {
    auto prev_lvp            = static_cast<LongLastValueAccumulation *>(&prev);
    auto current_lvp         = static_cast<LongLastValueAccumulation *>(&current);
    auto latest_accumulation = (current_lvp->GetLastValueTimeStamp().time_since_epoch() >=
                                prev_lvp->GetLastValueTimeStamp().time_since_epoch())
                                   ? current_lvp
                                   : prev_lvp;

    return std::move(std::unique_ptr<Accumulation>(new LongLastValueAccumulation(
        prev_lvp->ToPointData().value_, latest_accumulation->GetLastValueTimeStamp())));
  }

  /** Returns a new DELTA aggregation by comparing two cumulative measurements.*/
  std::unique_ptr<Accumulation> diff(Accumulation &prev, Accumulation &current) noexcept override
  {
    auto prev_lvp    = static_cast<LongLastValueAccumulation *>(&prev);
    auto current_lvp = static_cast<LongLastValueAccumulation *>(&current);
    return std::move(std::unique_ptr<Accumulation>(new LongLastValueAccumulation(
        prev_lvp->ToPointData().value_ - current_lvp->ToPointData().value_)));
  }

  LongSingularMetricData ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<LongSumAccumulation>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    LongSingularMetricData metrics_data = {resource, instrumentation_library,
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

class DoubleLastValueAggregator : public Aggregator
{
public:
  std::unique_ptr<Accumulation> CreateAccumulation() noexcept override
  {
    return std::move(std::unique_ptr<Accumulation>(new DoubleLastValueAccumulation()));
  }

  /** Returns the result of the merge of the given accumulations.*/

  std::unique_ptr<Accumulation> Merge(Accumulation &prev, Accumulation &current) noexcept override
  {
    auto prev_lvp            = static_cast<DoubleLastValueAccumulation *>(&prev);
    auto current_lvp         = static_cast<DoubleLastValueAccumulation *>(&current);
    auto latest_accumulation = (current_lvp->GetLastValueTimeStamp().time_since_epoch() >=
                                prev_lvp->GetLastValueTimeStamp().time_since_epoch())
                                   ? current_lvp
                                   : prev_lvp;

    return std::move(std::unique_ptr<Accumulation>(new DoubleLastValueAccumulation(
        prev_lvp->ToPointData().value_, latest_accumulation->GetLastValueTimeStamp())));
  }

  /** Returns a new DELTA aggregation by comparing two cumulative measurements.*/
  std::unique_ptr<Accumulation> diff(Accumulation &prev, Accumulation &current) noexcept override
  {
    auto prev_lvp    = static_cast<DoubleLastValueAccumulation *>(&prev);
    auto current_lvp = static_cast<DoubleLastValueAccumulation *>(&current);
    return std::move(std::unique_ptr<Accumulation>(new DoubleLastValueAccumulation(
        prev_lvp->ToPointData().value_ - current_lvp->ToPointData().value_)));
  }

  DoubleSingularMetricData ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<DoubleSumAccumulation>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    DoubleSingularMetricData metrics_data = {resource, instrumentation_library,
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