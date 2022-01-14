// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/last_value_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/noop_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/sum_aggregator.h"

#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template <class T>
class Aggregation
{
public:
  virtual ~Aggregation() = default;
  virtual opentelemetry::sdk::metrics::Aggregator<T> &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept = 0;
};

template <class T>
class SumAggregation : public Aggregation<T>
{
public:
  virtual opentelemetry::sdk::metrics::Aggregator<T> &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static SumAggregator<T> sum_aggregator;
    return sum_aggregator;
  }
};

template <class T>
class LastValueAggregation : public Aggregation<T>
{
public:
  virtual opentelemetry::sdk::metrics::Aggregator<T> &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static LastValueAggregator<T> last_value_aggregator;
    return last_value_aggregator;
  }
};

template <class T>
class NoopAggregation : public Aggregation<T>
{
  opentelemetry::sdk::metrics::Aggregator<T> &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static NoopAggregator<T> noop_aggregator;
    return noop_aggregator;
  }
};

template <class T>
class DefaultAggregation : public Aggregation<T>
{

  opentelemetry::sdk::metrics::Aggregator<T> &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    // TBD - This shouldn't return noop_aggregator
    static NoopAggregator<T> noop_aggregator;
    return noop_aggregator;
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif