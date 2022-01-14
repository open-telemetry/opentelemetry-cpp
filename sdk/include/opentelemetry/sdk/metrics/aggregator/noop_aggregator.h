// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
# include "opentelemetry/sdk/metrics/aggregator/accumulation.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
    /** Basic aggregator which calculates a Sum from individual measurements. */
    template <class T>
    class NoopAggregator: public Aggregator<NoopAccumulation<T>>
    {
        public:
        
            NoopAccumulation<T> CreateAccumulation() noexcept override {
                return NoopAccumulation<T>();
            }

            /** Returns the result of the merge of the given accumulations.*/
            NoopAccumulation<T> Merge(NoopAccumulation<T>& prev, NoopAccumulation<T>& current ) noexcept override {
                return SumAccumulation<T>();

            }

            /** Returns a new delta aggregation by comparing two cumulative measurements.*/
            NoopAccumulation<T> diff(NoopAccumulation<T>& prev, NoopAccumulation<T>& current) noexcept override {
                 return SumAccumulation<T>(prev.ToPointData().value_ - current.ToPointData().value_);
            }

            NoopMetricData<T> ToMetricData(
                opentelemetry::sdk::resource::Resource *resource,
                opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
                opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
                std::vector<opentelemetry::sdk::metrics::AccumulationRecord<SumAccumulation<T>>>& accumulation_by_attributes,
                opentelemetry::common::SystemTimestamp& start_epoch_ns,
                opentelemetry::common::SystemTimestamp& end_epoch_ns
            ){
                NoopMetricData<T> metrics_data = {resource, instrumentation_library, instrument_descriptor };
                return metrics_data;
            }
    };
}
}
OPENTELEMETRY_END_NAMESPACE
#endif