#include "opentelemetry/sdk/metrics/ungrouped_processor.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include <gtest/gtest.h>

namespace sdkmetrics = opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;
namespace nostd = opentelemetry::nostd;

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessShort)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)); 

  auto aggregator = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>> (new
       opentelemetry::sdk::metrics::CounterAggregator<short>(metrics_api::InstrumentKind::Counter));

  aggregator->update(4);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<short>>>(checkpoint[0].GetAggregator()), aggregator);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessInt)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)); 

  auto aggregator = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>> (new
       opentelemetry::sdk::metrics::CounterAggregator<int>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator()), aggregator);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessFloat)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)); 

  auto aggregator = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>> (new
       opentelemetry::sdk::metrics::CounterAggregator<float>(metrics_api::InstrumentKind::Counter));

  aggregator->update(8.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<float>>>(checkpoint[0].GetAggregator()), aggregator);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessDouble)
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
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator()), aggregator);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulDouble)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true)); 

  auto aggregator = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>> (new
       opentelemetry::sdk::metrics::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())->get_checkpoint(), aggregator->get_checkpoint());

  double prev_checkpoint = aggregator->get_checkpoint()[0];
  aggregator->update(5.4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(nostd::get<nostd::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())->get_checkpoint()[0], aggregator->get_checkpoint()[0] + prev_checkpoint);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorFinishedCollectionStateless)
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
  ASSERT_EQ(checkpoint.size(),1);
  processor->FinishedCollection();
  checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 0);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorFinishedCollectionStateful)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true)); 

  auto aggregator = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>> (new
       opentelemetry::sdk::metrics::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 1);
  processor->FinishedCollection();
  ASSERT_EQ(checkpoint.size(), 1);

}

TEST(UngroupedMetricsProcessor, UngroupedProcessorMultipleRecordsStateless)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor> (new
       opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false)); 

  auto agg_short = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>> (new
       opentelemetry::sdk::metrics::CounterAggregator<short>(metrics_api::InstrumentKind::Counter));
  
  auto agg_int = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>> (new
       opentelemetry::sdk::metrics::CounterAggregator<int>(metrics_api::InstrumentKind::Counter));
  
  auto agg_float = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>> (new
      opentelemetry::sdk::metrics::CounterAggregator<float>(metrics_api::InstrumentKind::Counter));

  auto agg_double = nostd::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>> (new
      opentelemetry::sdk::metrics::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  agg_short->update(1);
  agg_short->checkpoint();

  agg_int->update(2);
  agg_int->checkpoint();

  agg_float->update(3);
  agg_float->checkpoint();

  agg_double->update(4);
  agg_double->checkpoint();

  sdkmetrics::Record r1{"agg_short", "a counter aggregator with type short","x=y",agg_short};
  sdkmetrics::Record r2{"agg_int", "a counter aggregator with type int","y=x",agg_int};
  sdkmetrics::Record r3{"agg_float", "a counter aggregator with type float","y=y",agg_float};
  sdkmetrics::Record r4{"agg_double", "a counter aggregator with type double","x=x",agg_double};

  std::vector<sdkmetrics::Record> unprocessed_records = {r1, r2, r3, r4};
  for(auto record : unprocessed_records)
  {
    processor->process(record);
  }

  std::vector<sdkmetrics::Record> processed_records = processor->CheckpointSelf();
  ASSERT_EQ(processed_records.size(), unprocessed_records.size());
}
