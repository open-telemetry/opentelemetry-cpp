// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/histogram_aggregation.h"
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
  const std::vector<long> boundaries     = {0, 5, 10, 25, 50, 75, 100, 250, 500, 1000};
  std::unique_ptr<Aggregator> aggregator = nullptr;
  if (instrument_descriptor.valueType_ == InstrumentValueType::kLong)
  {
    const std::vector<long> boundaries = {0, 5, 10, 25, 50, 75, 100, 250, 500, 1000};
    aggregator.reset(new LongHistogramAggregator(boundaries));
  }
  else
  {
    const std::vector<double> boundaries = {0, 5, 10, 25, 50, 75, 100, 250, 500, 1000};
    aggregator.reset(new DoubleHistogramAggregator(boundaries));
  }
  return std::move(aggregator);
}

Aggregation &HistogramAggregation::GetInstance() noexcept
{
  static HistogramAggregation histogram_aggregation;
  return histogram_aggregation;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
