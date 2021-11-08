// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/_metrics/ungrouped_processor.h"
#  include <gtest/gtest.h>
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"

namespace metric_sdk  = opentelemetry::sdk::metrics;
namespace metrics_api = opentelemetry::metrics;
namespace nostd       = opentelemetry::nostd;

/* Test that CheckpointSelf() will return the amount of unique records in it, then
   call FinishedCollection and see the map reset */
TEST(UngroupedMetricsProcessor, UngroupedProcessorFinishedCollectionStateless)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  auto aggregator2 = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  aggregator2->update(500.4);
  aggregator2->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  // Must have different (name, description, label, instrument) to map to
  metric_sdk::Record r2("name2", "description2", "labels2", aggregator2);

  processor->process(r);
  processor->process(r2);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 2);

  processor->FinishedCollection();

  checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 0);
}

/* Test that CheckpointSelf() will return the amount of unique records in it, then
   call FinishedCollection and see the map stay the same */
TEST(UngroupedMetricsProcessor, UngroupedProcessorFinishedCollectionStateful)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  auto aggregator2 = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  aggregator2->update(500.4);
  aggregator2->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  // Must have different (name, description, label, instrument) to map to
  metric_sdk::Record r2("name2", "description2", "labels2", aggregator2);

  processor->process(r);
  processor->process(r2);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint.size(), 2);

  processor->FinishedCollection();

  ASSERT_EQ(checkpoint.size(), 2);
}

// Test to make sure we keep information from record(short) that goes through process()
TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessShort)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::CounterAggregator<short>(metrics_api::InstrumentKind::Counter));

  aggregator->update(4);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(checkpoint[0].GetAggregator()),
      aggregator);
}

// Test to make sure we keep information from record(int) that goes through process()
TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessInt)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::CounterAggregator<int>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator()),
            aggregator);
}

// Test to make sure we keep information from record(float) that goes through process()
TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessFloat)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<float>>(
      new metric_sdk::CounterAggregator<float>(metrics_api::InstrumentKind::Counter));

  aggregator->update(8.5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(checkpoint[0].GetAggregator()),
      aggregator);
}

// Test to make sure we keep information from record(double) that goes through process()
TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatelessDouble)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(false));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);
  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint[0].GetAggregator()),
      aggregator);
}

/**
 * The following tests are for the Stateful version of the processor. These tests will make sure
 * that when we send the same aggreagtor twice through process(), that the values will be merged.
 * We can easily recreate this expected value by making a test aggregator that is updated through
 * both process functions but only checkpointed at the end.
 */
TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulShort)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::CounterAggregator<short>(metrics_api::InstrumentKind::Counter));

  auto aggregator_test = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::CounterAggregator<short>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator_test->update(5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(4);
  aggregator_test->update(4);
  aggregator->checkpoint();
  aggregator_test->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint()[0],
      aggregator_test->get_checkpoint()[0]);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulInt)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::CounterAggregator<int>(metrics_api::InstrumentKind::Counter));

  auto aggregator_test = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::CounterAggregator<int>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator_test->update(5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_checkpoint(),
            aggregator->get_checkpoint());

  aggregator->update(4);
  aggregator_test->update(4);
  aggregator->checkpoint();
  aggregator_test->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_checkpoint()[0],
            aggregator_test->get_checkpoint()[0]);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulFloat)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<float>>(
      new metric_sdk::CounterAggregator<float>(metrics_api::InstrumentKind::Counter));

  auto aggregator_test = std::shared_ptr<metric_sdk::Aggregator<float>>(
      new metric_sdk::CounterAggregator<float>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5);
  aggregator_test->update(5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(4);
  aggregator_test->update(4);
  aggregator->checkpoint();
  aggregator_test->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint()[0],
      aggregator_test->get_checkpoint()[0]);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulDouble)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  auto aggregator_test = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator_test->update(5.5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(4.4);
  aggregator_test->update(4.4);
  aggregator->checkpoint();
  aggregator_test->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint()[0],
      aggregator_test->get_checkpoint()[0]);
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulMinMaxSumCount)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::MinMaxSumCountAggregator<double>(metrics_api::InstrumentKind::Counter));

  auto aggregator2 = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::MinMaxSumCountAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(1.1);
  aggregator->update(2.2);
  aggregator2->update(1.1);
  aggregator2->update(2.2);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(5.5);
  aggregator2->update(5.5);
  aggregator->checkpoint();
  aggregator2->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint2[0].GetAggregator())
          ->get_checkpoint(),
      aggregator2->get_checkpoint());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulGauge)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::GaugeAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(1.1);
  aggregator->update(2.2);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(5.4);
  aggregator->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint2[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulExact)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::ExactAggregator<double>(metrics_api::InstrumentKind::Counter, false));

  auto aggregator2 = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::ExactAggregator<double>(metrics_api::InstrumentKind::Counter, false));

  aggregator->update(1.1);
  aggregator->update(2.2);
  aggregator2->update(1.1);
  aggregator2->update(2.2);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint[0].GetAggregator())
          ->get_checkpoint(),
      aggregator->get_checkpoint());

  aggregator->update(5.4);
  aggregator2->update(5.4);
  aggregator->checkpoint();
  aggregator2->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();

  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(
      nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(checkpoint2[0].GetAggregator())
          ->get_checkpoint(),
      aggregator2->get_checkpoint());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulHistogram)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  std::vector<double> boundaries{10, 20, 30, 40, 50};
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::HistogramAggregator<int>(metrics_api::InstrumentKind::Counter, boundaries));

  auto aggregator2 = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::HistogramAggregator<int>(metrics_api::InstrumentKind::Counter, boundaries));

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
    aggregator2->update(i);
  }
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_boundaries(),
            aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_counts(),
            aggregator->get_counts());

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
    aggregator2->update(i);
  }
  aggregator->checkpoint();
  aggregator2->checkpoint();

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();

  ASSERT_EQ(checkpoint2.size(), 1);
  ASSERT_EQ(checkpoint2[0].GetName(), "name");
  ASSERT_EQ(checkpoint2[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint2[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_boundaries(),
            aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_counts(),
            aggregator2->get_counts());
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_checkpoint(),
            aggregator2->get_checkpoint());
}

TEST(UngroupedMetricsProcessor, UngroupedProcessorKeepsRecordInformationStatefulSketch)
{
  auto processor = std::unique_ptr<metric_sdk::MetricsProcessor>(
      new metric_sdk::UngroupedMetricsProcessor(true));

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::SketchAggregator<int>(metrics_api::InstrumentKind::Counter, .00005));

  auto test_aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::SketchAggregator<int>(metrics_api::InstrumentKind::Counter, .00005));

  for (int i = 0; i < 60; i++)
  {
    aggregator->update(i);
    test_aggregator->update(i);
  }
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);

  processor->process(r);

  std::vector<metric_sdk::Record> checkpoint = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint[0].GetName(), "name");
  ASSERT_EQ(checkpoint[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator())
                ->get_boundaries(),
            aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint[0].GetAggregator())
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

  std::vector<metric_sdk::Record> checkpoint2 = processor->CheckpointSelf();
  ASSERT_EQ(checkpoint2[0].GetName(), "name");
  ASSERT_EQ(checkpoint2[0].GetLabels(), "labels");
  ASSERT_EQ(checkpoint2[0].GetDescription(), "description");
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_boundaries(),
            test_aggregator->get_boundaries());
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_counts(),
            test_aggregator->get_counts());
  ASSERT_EQ(nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(checkpoint2[0].GetAggregator())
                ->get_checkpoint(),
            test_aggregator->get_checkpoint());
}
#endif
