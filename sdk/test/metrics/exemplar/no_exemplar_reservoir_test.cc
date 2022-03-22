// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

TEST(NoExemplarReservoir, OfferMeasurement)
{
  auto reservoir = opentelemetry::sdk::metrics::NoExemplarReservoir::GetNoExemplarReservoir();
  EXPECT_NO_THROW(reservoir->OfferMeasurement(1.0, MetricAttributes{},
                                              opentelemetry::context::Context{},
                                              std::chrono::system_clock::now()));
  EXPECT_NO_THROW(reservoir->OfferMeasurement(
      1l, MetricAttributes{}, opentelemetry::context::Context{}, std::chrono::system_clock::now()));
  auto exemplar_data = reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_TRUE(exemplar_data.empty());
}

#endif
