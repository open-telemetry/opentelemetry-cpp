// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#include "opentelemetry/sdk/metrics/point_data.h"
#  include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
    /**
     * A collection of data points associated to a metric.
     */
    template <class T>
    typename std::enable_if<std::is_base_of<PointData, T>::value>::type
    class Data : public T
    {
        public:
        virtual std::vector<T> GetPoints()= 0;

    };

    
}
}
OPENTELEMETRY_END_NAMESPACE
#endif
