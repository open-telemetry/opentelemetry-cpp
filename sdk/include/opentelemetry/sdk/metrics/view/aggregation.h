// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include "opentelemetry/sdk/common/global_log_handler.h"
#  include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/drop_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/historgram_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/last_value_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/sum_aggregator.h"
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
  virtual opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept = 0;
};

class DropAggregation : public Aggregation
{
  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::DropAggregator drop_aggregator;
    return drop_aggregator;
  }
};

class SumAggregation : public Aggregation
{
  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::SumAggregator sum_aggregator;
    return sum_aggregator;
  }
};

class LastValueAggregation : public Aggregation
{
  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::LastValueAggregator last_value_aggregator;
    return last_value_aggregator;
  }
};

class HistogramAggregation : public Aggregation
{
  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::HistogramAggregator histogram_aggregator;
    return histogram_aggregator;
  }
};

class DefaultAggregation : public Aggregation
{
  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::DropAggregator drop_aggregator;
    return drop_aggregator;
  }
};

class NoOpAggregation : public Aggregation
{

  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    static opentelemetry::sdk::metrics::NoOpAggregator noop_aggregator;
    return noop_aggregator;
  }
};

class DefaultAggregation : public Aggregation
{
  opentelemetry::sdk::metrics::Aggregator &CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor instrument_descriptor) noexcept override
  {
    return selectAggregation(instrument_descriptor).CreateAggregator(instrument_descriptor);
  }

private:
  opentelemetry::sdk::metrics::Aggregation &selectAggregation(
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor)
  {
    switch (instrument_descriptor.type_)
    {
      case InstrumentType::kCounter:
      case InstrumentType::kUpDownCounter:
      case InstrumentType::kObservableCounter:
      case InstrumentType::kObservableUpDownCounter: {
        static SumAggregation sum_aggregation;
        return sum_aggregation;
      }
      case InstrumentType::kObservableGauge: {
        static LastValueAggregation last_value_aggregation;
        return last_value_aggregation;
      }
      case InstrumentType::kHistogram: {
        static HistogramAggregation histogram_aggregation;
        return histogram_aggregation;
      }
      default: {
        OTEL_INTERNAL_LOG_ERROR("[Aggregation]: Unsupported InstrumentType ");
        static NoOpAggregation noop_aggregation;
        return noop_aggregation;
      }
    }
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif