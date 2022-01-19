// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/view/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/view/drop_aggregation.h"
#  include "opentelemetry/sdk/metrics/view/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/view/lastvalue_aggregation.h"
#  include "opentelemetry/sdk/metrics/view/sum_aggregation.h"

#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::unique_ptr<Aggregator> DefaultAggregation::CreateAggregator(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  return resolve(instrument_descriptor).CreateAggregator(instrument_descriptor);
}

opentelemetry::sdk::metrics::Aggregation &DefaultAggregation::resolve(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  switch (instrument_descriptor.type_)
  {
    case InstrumentType::kCounter:
    case InstrumentType::kUpDownCounter:
    case InstrumentType::kObservableUpDownCounter:
      return SumAggregation::GetInstance();
      break;
    case InstrumentType::kHistogram:
      return HistogramAggregation::GetInstance();
      break;
    case InstrumentType::kObservableGauge:
      return LastValueAggregation::GetInstance();
      break;
    default:
      // TBD - Return drop aggregation
      OTEL_INTERNAL_LOG_WARN(
          "[METRICS] Aggregation: Invalid Instrument Type - Using DropAggregation ");
      return DropAggregation::GetInstance();
  };
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif