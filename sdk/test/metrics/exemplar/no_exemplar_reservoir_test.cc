// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_EXEMPLAR_PREVIEW

#  include <gtest/gtest.h>
#  include <cstdint>
#  include <memory>
#  include <string>
#  include <vector>

#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#  include "opentelemetry/sdk/metrics/exemplar/reservoir.h"

using namespace opentelemetry::sdk::metrics;

TEST(NoExemplarReservoir, OfferMeasurement)
{
  auto reservoir = opentelemetry::sdk::metrics::ExemplarReservoir::GetNoExemplarReservoir();
  reservoir->OfferMeasurement(1.0, MetricAttributes{}, opentelemetry::context::Context{});
  reservoir->OfferMeasurement(static_cast<int64_t>(1), MetricAttributes{},
                              opentelemetry::context::Context{});
  auto exemplar_data = reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_TRUE(exemplar_data.empty());
}

#endif  // ENABLE_METRICS_EXEMPLAR_PREVIEW
