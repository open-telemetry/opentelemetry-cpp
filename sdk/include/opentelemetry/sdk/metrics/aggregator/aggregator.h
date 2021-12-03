// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
    /*
template <class T, class U>
class Aggregator {

public:
    virtual AggregatorHandle<T> createHandle()  = 0;
    T accumulateMeasurement(U value, common::KeyValueIterable &attributes){
         AggregatorHandle<T> handle = createHandle();
         handle.record(U value, attributes;
         return handle.accumulateThenReset(attributes);
    }
    virtual T merge(T previousCumulative, T delta)  = 0;
    virtual T diff(T previousCumulative, T currentCumulative) = 0;

};
*/
class Aggregator {
    //TBD
};

}
}
OPENTELEMETRY_END_NAMESPACE