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
class DropAggregator : public Aggregator
{
public:
  std::unique_ptr<Accumulation> CreateAccumulation() noexcept override
  {
    return std::move(std::unique_ptr<Accumulation>(new DropAccumulation()));
  }

  /** Returns the result of the merge of the given accumulations.*/
  std::unique_ptr<Accumulation> Merge(Accumulation &prev, Accumulation &current) noexcept override
  {
    return std::move(
        std::unique_ptr<Accumulation>(std::unique_ptr<Accumulation>(new DropAccumulation())));
  }

  /** Returns a new delta aggregation by comparing two cumulative measurements.*/
  std::unique_ptr<Accumulation> diff(Accumulation &prev, Accumulation &current) noexcept override
  {
    return std::move(
        std::unique_ptr<Accumulation>(std::unique_ptr<Accumulation>(new DropAccumulation())));
  }

  DropMetricData ToMetricData(
      opentelemetry::sdk::resource::Resource *resource,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
      std::vector<opentelemetry::sdk::metrics::AccumulationRecord<DropAccumulation>>
          &accumulation_by_attributes,
      opentelemetry::common::SystemTimestamp &start_epoch_ns,
      opentelemetry::common::SystemTimestamp &end_epoch_ns)
  {
    DropMetricData metrics_data;
    return metrics_data;
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif