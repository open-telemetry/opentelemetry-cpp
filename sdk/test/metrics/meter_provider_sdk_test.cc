// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"

using namespace opentelemetry::sdk::metrics;

TEST(MeterProvider, GetMeter)
{
  std::vector<std::unique_ptr<MeterExporter>> exporters;
  std::vector<std::unique_ptr<MeterReader>> readers;
  std::vector<std::unique_ptr<View>> views;
  MeterProvider mp(std::move(exporters), std::move(readers), std::move(views));
  auto t1 = mp.GetMeter("test");
  auto t2 = mp.GetMeter("test");
  auto t3 = mp.GetMeter("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_EQ(t1, t3);
}
#endif