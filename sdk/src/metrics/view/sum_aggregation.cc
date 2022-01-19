// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/sum_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregator/sum_aggregator.h"
#  include "opentelemetry/sdk/metrics/instruments.h"

#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
std::unique_ptr<Aggregator> SumAggregation::CreateAggregator(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  auto aggregator =
      (instrument_descriptor.valueType_ == InstrumentValueType::kLong)
          ? std::unique_ptr<Aggregator>(new LongSumAggregator(instrument_descriptor))
          : std::unique_ptr<Aggregator>(new DoubleSumAggregator(instrument_descriptor));
  return std::move(aggregator);
}

Aggregation &SumAggregation::GetInstance() noexcept
{
  static SumAggregation sum_aggregation;
  return sum_aggregation;
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif