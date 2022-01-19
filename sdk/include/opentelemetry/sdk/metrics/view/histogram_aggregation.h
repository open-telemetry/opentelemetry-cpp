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
class HistogramAggregation : public Aggregation
{
public:
  virtual std::unique_ptr<Aggregator> CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept override;

  static Aggregation &GetInstance() noexcept;

private:
  HistogramAggregation() = default;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif