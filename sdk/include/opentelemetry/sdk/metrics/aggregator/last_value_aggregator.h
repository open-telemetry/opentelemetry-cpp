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
    /** Basic aggregator which calculates a last value from individual measurements. */
    template <class T>
    class LastValueAggregator: public Aggregator<LastValueAccumulation<T>>
    {
        public:
        
            LastValueAccumulation<T> CreateAccumulation() noexcept override {
                return new LastValueAccumulation<T>();
            }

            /** Returns the result of the merge of the given accumulations.*/

            LastValueAccumulation<T> Merge(LastValueAccumulation<T>& previous, LastValueAccumulation<T>& current ) noexcept override {
                auto latest_accumulation =
                     (current.GetLastValueTimeStamp().time_since_epoch() >= previous.GetLastValueTimeStamp().time_since_epoch())?
                        current : previous;
                
                return new LastValueAccumulation<T>(latest_accumulation.toPointData().value_, latest_accumulation.GetLastValueTimeStamp());
            }

            /** Returns a new DELTA aggregation by comparing two cumulative measurements.*/
            LastValueAccumulation<T> diff(LastValueAccumulation<T>& prev, LastValueAccumulation<T>& current) noexcept override {
                 return LastValueAccumulation<T>(prev.ToPoint() - current.ToPoint());
            }

            SingularMetricsData<T> ToMetricData(
                opentelemetry::sdk::resource::Resource *resource,
                opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
                opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor,
                std::vector<opentelemetry::sdk::metrics::AccumulationRecord<SumAccumulation<T>>>& accumulation_by_attributes,
                opentelemetry::common::SystemTimestamp& start_epoch_ns,
                opentelemetry::common::SystemTimestamp& end_epoch_ns
            ){
                SingularMetricsData<T> metrics_data = {resource, instrumentation_library, instrument_descriptor };
                for (auto &acc_by_attr : accumulation_by_attributes){
                    opentelemetry::sdk::metrics::BasePointData base_point_data = {start_epoch_ns, end_epoch_ns, acc_by_attr.attributes};
                    auto point_data = acc_by_attr.accumulation.ToPointData();
                    metrics_data.point_data_list.push_back(std::make_pair(base_point_data, point_data));
                }
                return metrics_data;
            }
    };
}
}
OPENTELEMETRY_END_NAMESPACE
#endif