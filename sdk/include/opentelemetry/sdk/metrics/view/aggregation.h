#if 0
// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#  pragma once
#  ifndef ENABLE_METRICS_PREVIEW

#    include <memory>
#    include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#    include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Aggregation
{
public:
  virtual ~Aggregation() = default;
  virtual opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept = 0;
};

class NoOpAggregation : public Aggregation
{

  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::NoOpAggregator noop_aggregator;
    return noop_aggregator;
  }
};

class DefaultAggregation : public Aggregation
{

  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    // TBD - This shouldn't return noop_aggregator
    static opentelemetry::sdk::metrics::NoOpAggregator noop_aggregator;
    return noop_aggregator;
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#  endif
#endif