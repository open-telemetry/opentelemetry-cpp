// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Aggregation
{
public:
  virtual ~Aggregation() = default;
  virtual std::unique_ptr<opentelemetry::sdk::metrics::Aggregator> CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept = 0;
};

class DefaultAggregation : public Aggregation
{
  std::unique_ptr<opentelemetry::sdk::metrics::Aggregator> CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    return std::unique_ptr<Aggregator>();
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif