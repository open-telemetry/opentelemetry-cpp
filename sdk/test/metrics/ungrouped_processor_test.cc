#include "opentelemetry/sdk/metrics/ungrouped_processor.h"
#include <gtest/gtest.h>
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"

namespace sdkmetrics  = opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;
namespace nostd       = opentelemetry::nostd;

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessShort)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>(
      new opentelemetry::sdk::metrics::CounterAggregator<short>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(4);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(checkpoint[0].GetAggregator()),
      aggregator);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessInt)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
      new opentelemetry::sdk::metrics::CounterAggregator<int>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator()),
            aggregator);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessFloat)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>(
      new opentelemetry::sdk::metrics::CounterAggregator<float>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(8.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(checkpoint[0].GetAggregator()),
      aggregator);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessDouble)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::CounterAggregator<double>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator()),
      aggregator);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulShort)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>(
      new opentelemetry::sdk::metrics::CounterAggregator<short>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  short prev_checkpoint = aggregator->get_checkpoint()[0];
  aggregator->update(4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<short>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint()[0],
      aggregator->get_checkpoint()[0] + prev_checkpoint);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulInt)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
      new opentelemetry::sdk::metrics::CounterAggregator<int>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_checkpoint(),
            aggregator->get_checkpoint());

  int prev_checkpoint = aggregator->get_checkpoint()[0];
  aggregator->update(4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_checkpoint()[0],
            aggregator->get_checkpoint()[0] + prev_checkpoint);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulFloat)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>(
      new opentelemetry::sdk::metrics::CounterAggregator<float>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  float prev_checkpoint = aggregator->get_checkpoint()[0];
  aggregator->update(5.4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<float>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint()[0],
      aggregator->get_checkpoint()[0] + prev_checkpoint);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulDouble)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::CounterAggregator<double>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  double prev_checkpoint = aggregator->get_checkpoint()[0];
  aggregator->update(5.4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint()[0],
      aggregator->get_checkpoint()[0] + prev_checkpoint);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorFinishedCollectionStateless)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::CounterAggregator<double>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 1);
  processor->FinishedCollection();
  checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 0);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorFinishedCollectionStateful)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::CounterAggregator<double>(
          metrics_api::InstrumentKind::Counter));

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
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(false));

  auto agg_short = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<short>>(
      new opentelemetry::sdk::metrics::CounterAggregator<short>(
          metrics_api::InstrumentKind::Counter));

  auto agg_int = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
      new opentelemetry::sdk::metrics::CounterAggregator<int>(
          metrics_api::InstrumentKind::Counter));

  auto agg_float = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<float>>(
      new opentelemetry::sdk::metrics::CounterAggregator<float>(
          metrics_api::InstrumentKind::Counter));

  auto agg_double = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::CounterAggregator<double>(
          metrics_api::InstrumentKind::Counter));

  agg_short->update(1);
  agg_short->checkpoint();

  agg_int->update(2);
  agg_int->checkpoint();

  agg_float->update(3);
  agg_float->checkpoint();

  agg_double->update(4);
  agg_double->checkpoint();

  sdkmetrics::Record r1{"agg_short", "a counter aggregator with type short", "x=y", agg_short};
  sdkmetrics::Record r2{"agg_int", "a counter aggregator with type int", "y=x", agg_int};
  sdkmetrics::Record r3{"agg_float", "a counter aggregator with type float", "y=y", agg_float};
  sdkmetrics::Record r4{"agg_double", "a counter aggregator with type double", "x=x", agg_double};

  std::vector<sdkmetrics::Record> unprocessed_records = {r1, r2, r3, r4};
  for (auto record : unprocessed_records)
  {
    processor->process(record);
  }

  std::vector<sdkmetrics::Record> processed_records = processor->CheckpointSelf();
  ASSERT_EQ(processed_records.size(), unprocessed_records.size());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulMinMaxSumCount)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::MinMaxSumCountAggregator<double>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(1.1);
  aggregator->update(2.2);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulGauge)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::GaugeAggregator<double>(
          metrics_api::InstrumentKind::Counter));

  aggregator->update(1.1);
  aggregator->update(2.2);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(5.4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint2[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulExact)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<double>>(
      new opentelemetry::sdk::metrics::ExactAggregator<double>(metrics_api::InstrumentKind::Counter,
                                                               false));

  aggregator->update(1.1);
  aggregator->update(2.2);
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  std::vector<double> cumulative_checkpoint = aggregator->get_checkpoint();

  aggregator->update(5.4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  for (int i = 0; i < aggregator->get_checkpoint().size(); i++)
  {
    cumulative_checkpoint.push_back(aggregator->get_checkpoint()[i]);
  }
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<sdkmetrics::Aggregator<double>>>(checkpoint2[0].GetAggregator())
          ->get_checkpoint(),
      cumulative_checkpoint);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulHistogram)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  std::vector<double> boundaries{10, 20, 30, 40, 50};
  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
      new opentelemetry::sdk::metrics::HistogramAggregator<int>(
          metrics_api::InstrumentKind::Counter, boundaries));

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_boundaries(),
            aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_counts(),
            aggregator->get_counts());
  std::vector<int> prev_counts = aggregator->get_counts();

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  std::vector<int> new_counts                 = aggregator->get_counts();

  for (int i = 0; i < new_counts.size(); i++)
  {
    new_counts[i] += prev_counts[i];
  }
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(checkpoint2[0].GetName(), "name");
  ASSERT_EQ(checkpoint2[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint2[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_boundaries(),
            aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_counts(),
            new_counts);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulSketch)
{
  auto processor = std::unique_ptr<sdkmetrics::MetricsProcessor>(
      new opentelemetry::sdk::metrics::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
      new opentelemetry::sdk::metrics::SketchAggregator<int>(metrics_api::InstrumentKind::Counter,
                                                             .00005));

  auto test_aggregator = std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<int>>(
      new opentelemetry::sdk::metrics::SketchAggregator<int>(metrics_api::InstrumentKind::Counter,
                                                             .00005));

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
    test_aggregator->update(i);
  }
  aggregator->checkpoint();

  sdkmetrics::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_boundaries(),
            aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_counts(),
            aggregator->get_counts());

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
    test_aggregator->update(i);
  }
  aggregator->checkpoint();
  test_aggregator->checkpoint();

  processor->process(r);

  std::vector<sdkmetrics::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2[0].GetName(), "name");
  ASSERT_EQ(checkpoint2[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint2[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_boundaries(),
            test_aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<sdkmetrics::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_counts(),
            test_aggregator->get_counts());
}
