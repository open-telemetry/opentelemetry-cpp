// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/ostream/metrics_exporter.h"
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/exact_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/gauge_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/min_max_sum_count_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/sketch_aggregator.h"
#  include "opentelemetry/sdk/_metrics/exporter.h"
#  include "opentelemetry/sdk/_metrics/record.h"

#  include <iostream>

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace metrics_api     = opentelemetry::metrics;
namespace nostd           = opentelemetry::nostd;
namespace exportermetrics = opentelemetry::exporter::metrics;

TEST(OStreamMetricsExporter, PrintCounter)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter);

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metrics_api::InstrumentKind::Counter));

  aggregator->update(5.5);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  std::vector<metric_sdk::Record> records;
  records.push_back(r);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  sum         : 5.5\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, PrintMinMaxSumCount)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter);

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::MinMaxSumCountAggregator<int>(metrics_api::InstrumentKind::Counter));

  aggregator->update(1);
  aggregator->update(2);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  std::vector<metric_sdk::Record> records;
  records.push_back(r);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  min         : 1\n"
      "  max         : 2\n"
      "  sum         : 3\n"
      "  count       : 2\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, PrintGauge)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter);

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::GaugeAggregator<short>(metrics_api::InstrumentKind::Counter));

  aggregator->update(1);
  aggregator->update(9);
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  std::vector<metric_sdk::Record> records;
  records.push_back(r);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  last value  : 9\n"
      "  timestamp   : " +
      std::to_string(aggregator->get_checkpoint_timestamp().time_since_epoch().count()) +
      "\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, PrintExact)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter);

  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::ExactAggregator<short>(metrics_api::InstrumentKind::Counter, true));

  auto aggregator2 = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::ExactAggregator<short>(metrics_api::InstrumentKind::Counter, false));

  for (int i = 0; i < 10; i++)
  {
    aggregator->update(i);
    aggregator2->update(i);
  }
  aggregator->checkpoint();
  aggregator2->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  metric_sdk::Record r2("name", "description", "labels", aggregator2);
  std::vector<metric_sdk::Record> records;
  records.push_back(r);
  records.push_back(r2);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  quantiles   : [0: 0, .25: 3, .50: 5, .75: 7, 1: 9]\n"
      "}\n"
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  values      : [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, PrintHistogram)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter);

  std::vector<double> boundaries{10, 20, 30, 40, 50};
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<float>>(
      new metric_sdk::HistogramAggregator<float>(metrics_api::InstrumentKind::Counter, boundaries));

  for (float i = 0; i < 60; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  std::vector<metric_sdk::Record> records;
  records.push_back(r);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  buckets     : [10, 20, 30, 40, 50]\n"
      "  counts      : [10, 10, 10, 10, 10, 10]\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}

TEST(OStreamMetricsExporter, PrintSketch)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter);

  std::vector<double> boundaries{1, 3, 5, 7, 9};
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::SketchAggregator<int>(metrics_api::InstrumentKind::Counter, .000005));

  for (int i = 0; i < 10; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();

  metric_sdk::Record r("name", "description", "labels", aggregator);
  std::vector<metric_sdk::Record> records;
  records.push_back(r);

  // Create stringstream to redirect to
  std::stringstream stdoutOutput;

  // Save cout's buffer here
  std::streambuf *sbuf = std::cout.rdbuf();

  // Redirect cout to our stringstream buffer
  std::cout.rdbuf(stdoutOutput.rdbuf());

  exporter->Export(records);

  std::cout.rdbuf(sbuf);

  std::string expectedOutput =
      "{\n"
      "  name        : name\n"
      "  description : description\n"
      "  labels      : labels\n"
      "  buckets     : [0, 0.999995, 2, 3.00001, 4, 4.99999, 5.99997, 7.00003, 8.00003, 9]\n"
      "  counts      : [1, 1, 1, 1, 1, 1, 1, 1, 1, 1]\n"
      "}\n";

  ASSERT_EQ(stdoutOutput.str(), expectedOutput);
}
#else
TEST(OStreamMetricsExporter, DummyTest)
{
  // empty
}
#endif
