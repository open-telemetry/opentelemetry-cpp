#include "opentelemetry/sdk/metrics/ungrouped_processor.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

TEST(UngroupedMetricsProcessor, GetMeter)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::exporter::metrics::UngroupedMetricsProcessor);

}