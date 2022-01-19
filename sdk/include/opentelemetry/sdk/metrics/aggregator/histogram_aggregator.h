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
/**
 * Basic aggregator which observes events and counts them in pre-defined buckets
 * and provides the total sum and count of all observations.
 */
class LongHistogramAggregator : public Aggregator
{
  // TBD - This class is placeholder, and needs to be implemented
public:
  LongHistogramAggregator(const std::vector<long> &boundaries) : boundaries_(boundaries) {}
  std::unique_ptr<Accumulation> CreateAccumulation() noexcept override
  {
    // TBD
    return std::move(std::unique_ptr<Accumulation>(new LongHistogramAccumulation(boundaries_)));
  }

  /** Returns the result of the merge of the given accumulations.*/
  std::unique_ptr<Accumulation> Merge(Accumulation &prev, Accumulation &current) noexcept override
  {
    // TBD
    return std::move(std::unique_ptr<Accumulation>(new LongHistogramAccumulation(boundaries_)));
  }

  /** Returns a new delta aggregation by comparing two cumulative measurements.*/
  std::unique_ptr<Accumulation> diff(Accumulation &prev, Accumulation &current) noexcept override
  {
    return std::move(std::unique_ptr<Accumulation>(new LongHistogramAccumulation(boundaries_)));
  }

  LongHistogramMetricData ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<LongSumAccumulation>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    LongHistogramMetricData metrics_data = {resource, instrumentation_library,
                                            instrument_descriptor};
    // TBD - Populate me
    return metrics_data;
  }

private:
  std::vector<long> boundaries_;
};

class DoubleHistogramAggregator : public Aggregator
{
  // TBD - This class is placeholder, and needs to be implemented
public:
  DoubleHistogramAggregator(const std::vector<double> &boundaries) : boundaries_(boundaries) {}
  std::unique_ptr<Accumulation> CreateAccumulation() noexcept override
  {
    return std::move(std::unique_ptr<Accumulation>(new DoubleHistogramAccumulation(boundaries_)));
  }

  /** Returns the result of the merge of the given accumulations.*/
  std::unique_ptr<Accumulation> Merge(Accumulation &prev, Accumulation &current) noexcept override
  {
    // TBD
    return std::move(std::unique_ptr<Accumulation>(new DoubleHistogramAccumulation(boundaries_)));
  }

  /** Returns a new delta aggregation by comparing two cumulative measurements.*/
  std::unique_ptr<Accumulation> diff(Accumulation &prev, Accumulation &current) noexcept override
  {
    // TBD
    return std::move(std::unique_ptr<Accumulation>(new DoubleHistogramAccumulation(boundaries_)));
  }

  DoubleHistogramMetricData ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<DoubleSumAccumulation>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    DoubleHistogramMetricData metrics_data = {resource, instrumentation_library,
                                              instrument_descriptor};
    // TBD - Populate me
    return metrics_data;
  }

private:
  std::vector<double> boundaries_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif