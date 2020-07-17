#include "opentelemetry/sdk/metrics/exporter.h"
#include "opentelemetry/exporters/stream/metrics_exporter.h"

#include <iostream>
#include <gtest/gtest.h>

namespace sdkmetrics = opentelemetry::sdk::metrics;

TEST(StreamMetricsExporter, Shutdown)
{
  auto exporter = std::unique_ptr<sdkmetrics::MetricsExporter> (new
      opentelemetry::exporter::metrics::StreamMetricsExporter);
}