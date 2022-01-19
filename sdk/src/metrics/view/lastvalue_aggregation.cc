// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/lastvalue_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregator/lastvalue_aggregator.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::unique_ptr<Aggregator> LastValueAggregation::CreateAggregator(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  auto aggregator = (instrument_descriptor.valueType_ == InstrumentValueType::kLong)
                        ? std::unique_ptr<Aggregator>(new LongLastValueAggregator())
                        : std::unique_ptr<Aggregator>(new DoubleLastValueAggregator());
  return std::move(aggregator);
}

Aggregation &LastValueAggregation::GetInstance() noexcept
{
  static LastValueAggregation lastvalue_aggregation;
  return lastvalue_aggregation;
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif