// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/view/drop_aggregation.h"
#  include "opentelemetry/sdk/view/histogram_aggregation.h"
#  include "opentelemetry/sdk/view/lastvalue_aggregation.h"
#  include "opentelemetry/sdk/view/sum_aggregation.h"

#  include "opentelemetry/sdk_config.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

std::unique_ptr<Aggregator> DefaultAggregator::CreateAggregator(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept
{
  return resolve(instrument_descriptor).CreateAggregator(instrument_descriptor);
}

static opentelemetry::sdk::metrics::Aggregation &DefaultAggregator::resolve(
    opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept;
{
  switch (instrument_descriptor.type_)
  {
    case InstrumentType.kCounter:
    case InstrumentType.kUpDownCounter:
    case InstrumentType.kUpDownCounter:
    case InstrumentType.kObservableUpDownCounter:
      return SumAggregation::GetInstance();
      break;
    case InstrumentType.kHistogram:
      return HistogramAggregation::GetInstance();
      break;
    case InstrumentType.kObservableGauge:
      return LastValueAggregation::GetInstance();
      break;
    default:
      // TBD - Return drop aggregation
      OTEL_INTERNAL_LOG_WARNING("[METRICS] Aggregation: Instrument Type - "
                                << static_cast<int>(instrument_descriptor.type_))
          << " - not supported. NoopAggregation will be used." return DropAggregation::GetInstance()
                 .CreateAggregator(instrument_descriptor);
  };
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif