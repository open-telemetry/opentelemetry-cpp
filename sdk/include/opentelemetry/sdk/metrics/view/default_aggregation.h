// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/view/aggregation.h"

#  include <memory>

class DefaultAggregation : public Aggregation
{
public:
  std::unique_ptr<Aggregator> CreateAggregator(
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept override;

private:
  static opentelemetry::sdk::metrics::Aggregation &resolve(
      opentelemetry::sdk::metrics::InstrumentDescriptor &instrument_descriptor) noexcept;
};
}
}
OPENTELEMETRY_END_NAMESPACE
#endif
