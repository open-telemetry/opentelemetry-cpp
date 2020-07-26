#include "opentelemetry/sdk/metrics/ungrouped_processor.h"
#include <gtest/gtest.h>


TEST(UngroupedMetricsProcessor, GetMeter)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true)); 

}