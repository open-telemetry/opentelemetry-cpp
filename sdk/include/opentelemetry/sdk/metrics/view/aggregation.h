// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE

#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include<memory>

namespace sdk
{
namespace metrics
{

template <class T>
class Aggregation {
    public:
        virtual ~Aggregation() = default;
        virtual std::shared_ptr<Aggregator> CreateAggregator( InstrumentDescriptor<T> instrumentDescriptor) = 0;

};
}
}
OPENTELEMETRY_END_NAMESPACE