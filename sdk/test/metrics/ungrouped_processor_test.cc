#include "opentelemetry/sdk/metrics/ungrouped_processor.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include <gtest/gtest.h>

namespace sdkmetrics = opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;
namespace nostd = opentelemetry::nostd;

TEST(UngroupedMetricsProcessor, GetMeter)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)); 

  auto aggregator = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>> (new
       opentelemetry::sdk::metrics::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 1) << checkpoint.size();
  
}