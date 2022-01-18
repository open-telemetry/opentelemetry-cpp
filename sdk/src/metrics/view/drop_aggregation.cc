// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/view/drop_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregator/drop_aggregator.h"
#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::unique_ptr<Aggregator> DropAggregation::CreateAggregator(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  return std::move(std::unique_ptr<Aggregator>(new DropAggregator<long>(instrument_descriptor)));
}

static Aggregation &DropAggregation::GetInstance()
{
  static DropAggregation drop_aggregation;
  return drop_aggregation;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif