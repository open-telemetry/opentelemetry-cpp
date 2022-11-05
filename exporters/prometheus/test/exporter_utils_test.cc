// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

#include "opentelemetry/exporters/prometheus/exporter_utils.h"
#include "prometheus_test_helper.h"

using opentelemetry::exporter::metrics::PrometheusExporterUtils;
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
    case prometheus_client::MetricType::Histogram: {
      ASSERT_DOUBLE_EQ(metric_data.histogram.sample_count, vals[0]);
      ASSERT_DOUBLE_EQ(metric_data.histogram.sample_sum, vals[1]);
      auto buckets = metric_data.histogram.bucket;
      ASSERT_EQ(buckets.size(), vals[2]);
      break;
    }
    case prometheus_client::MetricType::Gauge: {
      ASSERT_DOUBLE_EQ(metric_data.gauge.value, vals[0]);
      break;
    }
    break;
    case prometheus_client::MetricType::Summary:
      // Summary type not supported
      ASSERT_TRUE(false);
      break;
    case prometheus::MetricType::Untyped:
      break;
  }
}

void assert_histogram(prometheus_client::MetricFamily &metric,
                      std::list<double> boundaries,
                      std::vector<int> correct)
{
  int cumulative_count = 0;
  auto buckets         = metric.metric[0].histogram.bucket;
  auto boundary_it     = boundaries.cbegin();
  for (size_t i = 0; i < buckets.size(); i++, ++boundary_it)
  {
    auto bucket = buckets[i];
    if (i != buckets.size() - 1)
    {
      ASSERT_DOUBLE_EQ(*boundary_it, bucket.upper_bound);
    }
    else
    {
      ASSERT_DOUBLE_EQ(std::numeric_limits<double>::infinity(), bucket.upper_bound);
    }
    cumulative_count += correct[i];
    ASSERT_EQ(cumulative_count, bucket.cumulative_count);
  }
}

TEST(PrometheusExporterUtils, TranslateToPrometheusEmptyInputReturnsEmptyCollection)
{
  auto translated = PrometheusExporterUtils::TranslateToPrometheus(
      std::vector<std::unique_ptr<metric_sdk::ResourceMetrics>>{});
  ASSERT_EQ(translated.size(), 0);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerCounter)
{
  std::vector<std::unique_ptr<metric_sdk::ResourceMetrics>> collection;

  collection.emplace_back(new metric_sdk::ResourceMetrics{CreateSumPointData()});

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", prometheus_client::MetricType::Counter, 1,
               vals);

  collection.emplace_back(new metric_sdk::ResourceMetrics{CreateSumPointData()});

  translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric2 = translated[1];
  assert_basic(metric2, "library_name", "description", prometheus_client::MetricType::Counter, 1,
               vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerLastValue)
{
  std::vector<std::unique_ptr<metric_sdk::ResourceMetrics>> collection;

  collection.emplace_back(new metric_sdk::ResourceMetrics{CreateLastValuePointData()});

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", prometheus_client::MetricType::Gauge, 1,
               vals);

  collection.emplace_back(new metric_sdk::ResourceMetrics{CreateLastValuePointData()});

  translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric2 = translated[1];
  assert_basic(metric2, "library_name", "description", prometheus_client::MetricType::Gauge, 1,
               vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramNormal)
{
  std::vector<std::unique_ptr<metric_sdk::ResourceMetrics>> collection;

  collection.emplace_back(new metric_sdk::ResourceMetrics{CreateHistogramPointData()});

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(collection);
  ASSERT_EQ(translated.size(), collection.size());

  auto metric              = translated[0];
  std::vector<double> vals = {3, 900.5, 4};
  assert_basic(metric, "library_name", "description", prometheus_client::MetricType::Histogram, 1,
               vals);
  assert_histogram(metric, std::list<double>{10.1, 20.2, 30.2}, {200, 300, 400, 500});
}

OPENTELEMETRY_END_NAMESPACE
