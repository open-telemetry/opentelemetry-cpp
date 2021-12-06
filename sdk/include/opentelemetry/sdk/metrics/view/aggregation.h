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
  virtual std::shared_ptr<opentelemetry::sdk::metrics::Aggregator> CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept = 0;
};

class DefaultAggregation : public Aggregation
{};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif