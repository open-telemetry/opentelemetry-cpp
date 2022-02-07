// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/ostream/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"

#  include <iostream>

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace exportermetrics = opentelemetry::exporter::metrics;

TEST(OStreamMetricsExporter, Shutdown)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);
  ASSERT_TRUE(exporter->Shutdown());
  auto result =
      exporter->Export(nostd::span<std::unique_ptr<opentelemetry::sdk::metrics::Recordable>>{});
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

#endif
