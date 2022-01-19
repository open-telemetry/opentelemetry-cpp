// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/view/aggregation.h"

#  include <memory>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class DefaultAggregation : public Aggregation
{
public:
  std::unique_ptr<Aggregator> CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept override;

private:
  opentelemetry::sdk::metrics::Aggregation &resolve(
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
