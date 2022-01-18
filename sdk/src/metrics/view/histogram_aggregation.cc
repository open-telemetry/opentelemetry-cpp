// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/view/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregator/histogram_aggregator.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::unique_ptr<Aggregator> HistogramAggregation::CreateAggregator(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  // default boundaries -
  // https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/metrics/sdk.md#explicit-bucket-histogram-aggregation
  const std::vector<double> boundaries = {0, 5, 10, 25, 50, 75, 100, 250, 500, 1000};
  auto aggregator = (instrument_descriptor.valueType_ == InstrumentValueType.kLong)
                        ? std::unique_ptr<Aggregator>(new HitogramAggregation<long>(boundaries))
                        : std::unique_ptr<Aggregator>(new HistogramAggregation<double>(boundaries));
  return std::move(aggregator);
}

static Aggregation &HistogramAggregation::GetInstance() noexcept
{
  static LastValueAggregation lastvalue_aggregation;
  return lastvalue_aggregation;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
