// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h"
#include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

TEST(NoExemplarReservoir, OfferMeasurement)
{
  auto reservoir = opentelemetry::sdk::metrics::ExemplarReservoir::GetNoExemplarReservoir();
  reservoir->OfferMeasurement(1.0, MetricAttributes{}, opentelemetry::context::Context{},
                              std::chrono::system_clock::now());
  reservoir->OfferMeasurement((int64_t)1, MetricAttributes{}, opentelemetry::context::Context{},
                              std::chrono::system_clock::now());
  auto exemplar_data = reservoir->CollectAndReset(MetricAttributes{});
  ASSERT_TRUE(exemplar_data.empty());
}
