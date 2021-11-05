// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include <map>
#  include <numeric>
#  include <string>
#  include <typeinfo>

#  include <opentelemetry/version.h>
#  include "opentelemetry/exporters/prometheus/prometheus_exporter_utils.h"
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/exact_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/gauge_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/histogram_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/min_max_sum_count_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/sketch_aggregator.h"

using opentelemetry::exporter::prometheus::PrometheusExporterUtils;
namespace metric_sdk        = opentelemetry::sdk::metrics;
namespace metric_api        = opentelemetry::metrics;
namespace prometheus_client = ::prometheus;

OPENTELEMETRY_BEGIN_NAMESPACE
template <typename T>
void assert_basic(prometheus_client::MetricFamily &metric,
                  const std::string &sanitized_name,
                  const std::string &description,
                  prometheus_client::MetricType type,
                  int label_num,
                  std::vector<T> vals)
{
  ASSERT_EQ(metric.name, sanitized_name);  // name sanitized
  ASSERT_EQ(metric.help, description);     // description not changed
  ASSERT_EQ(metric.type, type);            // type translated

  auto metric_data = metric.metric[0];
  ASSERT_EQ(metric_data.label.size(), label_num);

  switch (type)
  {
    case prometheus_client::MetricType::Counter: {
      ASSERT_DOUBLE_EQ(metric_data.counter.value, vals[0]);
      break;
    }
    case prometheus_client::MetricType::Gauge: {
      ASSERT_EQ(metric_data.gauge.value, vals[0]);
      break;
    }
    case prometheus_client::MetricType::Histogram: {
      ASSERT_DOUBLE_EQ(metric_data.histogram.sample_count, vals[0]);
      ASSERT_DOUBLE_EQ(metric_data.histogram.sample_sum, vals[1]);
      auto buckets = metric_data.histogram.bucket;
      ASSERT_EQ(buckets.size(), vals[2]);
      break;
    }
    case prometheus_client::MetricType::Summary: {
      ASSERT_DOUBLE_EQ(metric_data.summary.sample_count, vals[0]);
      ASSERT_DOUBLE_EQ(metric_data.summary.sample_sum, vals[1]);
      break;
    }
    case prometheus::MetricType::Untyped:
      break;
  }
}

void assert_histogram(prometheus_client::MetricFamily &metric,
                      std::vector<double> boundaries,
                      std::vector<int> correct)
{
  int cumulative_count = 0;
  auto buckets         = metric.metric[0].histogram.bucket;
  for (size_t i = 0; i < buckets.size(); i++)
  {
    auto bucket = buckets[i];
    if (i != buckets.size() - 1)
    {
      ASSERT_DOUBLE_EQ(boundaries[i], bucket.upper_bound);
    }
    else
    {
      ASSERT_DOUBLE_EQ(std::numeric_limits<double>::infinity(), bucket.upper_bound);
    }
    cumulative_count += correct[i];
    ASSERT_EQ(cumulative_count, bucket.cumulative_count);
  }
}

template <typename T>
metric_sdk::Record get_record(const std::string &type,
                              int version,
                              const std::string &label,
                              std::shared_ptr<metric_sdk::Aggregator<T>> aggregator)
{
  std::string name = "test-" + type + "-metric-record-v_" + std::to_string(version) + ".0";
  std::string desc = "this is a test " + type + " metric record";
  metric_sdk::Record record(name, desc, label, aggregator);
  return record;
}

TEST(PrometheusExporterUtils, TranslateToPrometheusEmptyInputReturnsEmptyCollection)
{
  std::vector<metric_sdk::Record> collection;
  auto translated2 = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated2.size(), 0);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerCounter)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::CounterAggregator<int>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;

  auto record1 = get_record("int-counter", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  aggregator->update(10);
  aggregator->checkpoint();
  collection.emplace_back(record1);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "test_int_counter_metric_record_v_1_0", record1.GetDescription(),
               prometheus_client::MetricType::Counter, 3, vals);

  auto record2 = get_record("int-counter", 2, "{,}", aggregator);
  aggregator->update(20);
  aggregator->update(30);
  aggregator->checkpoint();
  collection.emplace_back(record2);

  translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric2 = translated[1];
  vals         = {50};
  assert_basic(metric2, "test_int_counter_metric_record_v_2_0", record2.GetDescription(),
               prometheus_client::MetricType::Counter, 0, vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusDoubleCounter)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;
  aggregator->update(10.5);
  aggregator->checkpoint();
  auto record1 = get_record("double-counter", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  aggregator->update(22.4);
  aggregator->update(31.2);
  aggregator->checkpoint();
  auto record2 = get_record("double-counter", 2, "{,}", aggregator);
  collection.emplace_back(record1);
  collection.emplace_back(record2);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1             = translated[0];
  std::vector<double> vals = {53.6};
  assert_basic(metric1, "test_double_counter_metric_record_v_1_0", record1.GetDescription(),
               prometheus_client::MetricType::Counter, 3, vals);
  auto metric2 = translated[1];
  assert_basic(metric2, "test_double_counter_metric_record_v_2_0", record2.GetDescription(),
               prometheus_client::MetricType::Counter, 0, vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusShortCounter)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<short>>(
      new metric_sdk::CounterAggregator<short>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;
  aggregator->update(10);
  aggregator->checkpoint();
  auto record1 = get_record("short-counter", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  aggregator->update(20);
  aggregator->update(30);
  aggregator->checkpoint();
  auto record2 = get_record("short-counter", 2, "{,}", aggregator);
  collection.emplace_back(record1);
  collection.emplace_back(record2);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1            = translated[0];
  std::vector<short> vals = {50};
  assert_basic(metric1, "test_short_counter_metric_record_v_1_0", record1.GetDescription(),
               prometheus_client::MetricType::Counter, 3, vals);
  auto metric2 = translated[1];
  assert_basic(metric2, "test_short_counter_metric_record_v_2_0", record2.GetDescription(),
               prometheus_client::MetricType::Counter, 0, vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusFloatCounter)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<float>>(
      new metric_sdk::CounterAggregator<float>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;
  aggregator->update(10.5f);
  aggregator->checkpoint();
  auto record1 = get_record("float-counter", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  aggregator->update(22.4f);
  aggregator->update(31.2f);
  aggregator->checkpoint();
  auto record2 = get_record("float-counter", 2, "{,}", aggregator);
  collection.emplace_back(record1);
  collection.emplace_back(record2);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1            = translated[0];
  std::vector<float> vals = {53.6f};
  assert_basic(metric1, "test_float_counter_metric_record_v_1_0", record1.GetDescription(),
               prometheus_client::MetricType::Counter, 3, vals);
  auto metric2 = translated[1];
  assert_basic(metric2, "test_float_counter_metric_record_v_2_0", record2.GetDescription(),
               prometheus_client::MetricType::Counter, 0, vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusGauge)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::GaugeAggregator<int>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;
  aggregator->update(10);
  aggregator->checkpoint();
  auto record1 = get_record("gauge", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  aggregator->update(20);
  aggregator->update(30);
  aggregator->checkpoint();
  auto record2 = get_record("gauge", 2, "{,}", aggregator);
  collection.emplace_back(record1);
  collection.emplace_back(record2);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1          = translated[0];
  std::vector<int> vals = {30};
  assert_basic(metric1, "test_gauge_metric_record_v_1_0", record1.GetDescription(),
               prometheus_client::MetricType::Gauge, 3, vals);
  auto metric2 = translated[1];
  assert_basic(metric2, "test_gauge_metric_record_v_2_0", record2.GetDescription(),
               prometheus_client::MetricType::Gauge, 0, vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramUniform)
{
  std::vector<double> boundaries{10, 20, 30, 40, 50};
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::HistogramAggregator<int>(metric_api::InstrumentKind::Counter, boundaries));

  std::vector<metric_sdk::Record> collection;
  auto record = get_record("histogram-uniform", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  int count_num = 60;
  for (int i = 0; i < count_num; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();
  collection.emplace_back(record);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric           = translated[0];
  std::vector<int> vals = {aggregator->get_checkpoint()[1], aggregator->get_checkpoint()[0],
                           (int)boundaries.size() + 1};
  assert_basic(metric, "test_histogram_uniform_metric_record_v_1_0", record.GetDescription(),
               prometheus_client::MetricType::Histogram, 3, vals);
  std::vector<int> correct = aggregator->get_counts();
  assert_histogram(metric, boundaries, correct);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramNormal)
{
  std::vector<double> boundaries{2, 4, 6, 8, 10, 12};
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::HistogramAggregator<int>(metric_api::InstrumentKind::Counter, boundaries));

  std::vector<metric_sdk::Record> collection;
  auto record = get_record("histogram-normal", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  std::vector<int> values{1, 3, 3, 5, 5, 5, 7, 7, 7, 7, 9, 9, 9, 11, 11, 13};
  for (int i : values)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();
  collection.emplace_back(record);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric           = translated[0];
  std::vector<int> vals = {aggregator->get_checkpoint()[1], aggregator->get_checkpoint()[0],
                           (int)boundaries.size() + 1};
  assert_basic(metric, "test_histogram_normal_metric_record_v_1_0", record.GetDescription(),
               prometheus_client::MetricType::Histogram, 3, vals);
  std::vector<int> correct = aggregator->get_counts();
  assert_histogram(metric, boundaries, correct);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusExact)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::ExactAggregator<int>(metric_api::InstrumentKind::Counter, true));

  std::vector<metric_sdk::Record> collection;
  int count_num = 100;
  for (int i = 0; i <= count_num; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();
  auto record = get_record("exact", 1, "{label-1:v1,label_2:v2,label3:v3,}", aggregator);
  collection.emplace_back(record);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric           = translated[0];
  std::vector<int> vals = {101, 5050};
  assert_basic(metric, "test_exact_metric_record_v_1_0", record.GetDescription(),
               prometheus_client::MetricType::Summary, 3, vals);
  auto quantile = metric.metric[0].summary.quantile;
  ASSERT_EQ(quantile.size(), 6);
  ASSERT_DOUBLE_EQ(quantile[0].value, 0);
  ASSERT_DOUBLE_EQ(quantile[1].value, 50);
  ASSERT_DOUBLE_EQ(quantile[2].value, 90);
  ASSERT_DOUBLE_EQ(quantile[3].value, 95);
  ASSERT_DOUBLE_EQ(quantile[4].value, 99);
  ASSERT_DOUBLE_EQ(quantile[5].value, 100);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusExactNoQuantile)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::ExactAggregator<int>(metric_api::InstrumentKind::Counter, false));

  std::vector<metric_sdk::Record> collection;
  int count_num = 10;
  for (int i = 0; i < count_num; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();
  auto record = get_record("exact-no-quantile", 1, "{label1:v1,label2:v2,}", aggregator);
  collection.emplace_back(record);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric           = translated[0];
  std::vector<int> vals = {count_num, 45};
  assert_basic(metric, "test_exact_no_quantile_metric_record_v_1_0", record.GetDescription(),
               prometheus_client::MetricType::Summary, 2, vals);
  auto quantile = metric.metric[0].summary.quantile;
  ASSERT_EQ(quantile.size(), 0);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusMinMaxSumCount)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::MinMaxSumCountAggregator<int>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;
  // min: 1, max: 10, sum: 55, count: 10
  for (int i = 1; i <= 10; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();
  auto record = get_record("mmsc", 1, "{label1:v1,label2:v2,label3:v3,}", aggregator);
  collection.emplace_back(record);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric = translated[0];
  // in this version of implementation, we use the sum/count as a gauge
  std::vector<double> vals = {5.5};
  assert_basic(metric, "test_mmsc_metric_record_v_1_0", record.GetDescription(),
               prometheus_client::MetricType::Gauge, 3, vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusSketch)
{
  auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::SketchAggregator<int>(metric_api::InstrumentKind::Counter, 0.0005));

  std::vector<metric_sdk::Record> collection;
  for (int i = 0; i <= 100; i++)
  {
    aggregator->update(i);
  }
  aggregator->checkpoint();
  auto record = get_record("sketch", 1, "{label1:v1,label2:v2,}", aggregator);
  collection.emplace_back(record);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric           = translated[0];
  std::vector<int> vals = {aggregator->get_checkpoint()[1], aggregator->get_checkpoint()[0]};
  assert_basic(metric, "test_sketch_metric_record_v_1_0", record.GetDescription(),
               prometheus_client::MetricType::Summary, 2, vals);

  auto quantile = metric.metric[0].summary.quantile;
  ASSERT_EQ(quantile.size(), 6);
  ASSERT_DOUBLE_EQ(quantile[0].value, 0);
  ASSERT_DOUBLE_EQ(quantile[1].value, 49);
  ASSERT_DOUBLE_EQ(quantile[2].value, 89);
  ASSERT_DOUBLE_EQ(quantile[3].value, 94);
  ASSERT_DOUBLE_EQ(quantile[4].value, 98);
  ASSERT_DOUBLE_EQ(quantile[5].value, 99);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusMultipleAggregators)
{
  auto counter_aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
      new metric_sdk::CounterAggregator<double>(metric_api::InstrumentKind::Counter));
  auto gauge_aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
      new metric_sdk::GaugeAggregator<int>(metric_api::InstrumentKind::Counter));

  std::vector<metric_sdk::Record> collection;
  counter_aggregator->update(10);
  counter_aggregator->update(20);
  counter_aggregator->checkpoint();
  auto record1 = get_record("counter", 1, "{label1:v1,label2:v2,label3:v3,}", counter_aggregator);
  gauge_aggregator->update(10);
  gauge_aggregator->update(30);
  gauge_aggregator->update(20);
  gauge_aggregator->checkpoint();
  auto record2 = get_record("gauge", 1, "{label1:v1,}", gauge_aggregator);
  collection.emplace_back(record1);
  collection.emplace_back(record2);

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1          = translated[0];
  std::vector<int> vals = {30};
  assert_basic(metric1, "test_counter_metric_record_v_1_0", record1.GetDescription(),
               prometheus_client::MetricType::Counter, 3, vals);
  auto metric2 = translated[1];
  vals         = {20};
  assert_basic(metric2, "test_gauge_metric_record_v_1_0", record2.GetDescription(),
               prometheus_client::MetricType::Gauge, 1, vals);
}
OPENTELEMETRY_END_NAMESPACE
#endif
