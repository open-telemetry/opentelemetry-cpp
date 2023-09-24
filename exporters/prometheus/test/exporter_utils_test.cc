// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

#include "opentelemetry/exporters/prometheus/exporter_utils.h"
#include "prometheus_test_helper.h"

OPENTELEMETRY_BEGIN_NAMESPACE

using exporter::metrics::TranslateToPrometheus;
namespace metric_sdk        = sdk::metrics;
namespace metric_api        = metrics;

template <typename T>
void assert_basic(::prometheus::MetricFamily &metric,
                  const std::string &sanitized_name,
                  const std::string &description,
                  ::prometheus::MetricType type,
                  int label_num,
                  std::vector<T> vals)
{
  ASSERT_EQ(metric.name, sanitized_name + "_unit");  // name sanitized
  ASSERT_EQ(metric.help, description);               // description not changed
  ASSERT_EQ(metric.type, type);                      // type translated

  // Prometheus metric data points should not have explicit timestamps
  for (const prometheus::ClientMetric &cm : metric.metric)
  {
    ASSERT_EQ(cm.timestamp_ms, 0);
  }

  auto metric_data = metric.metric[0];
  ASSERT_EQ(metric_data.label.size(), label_num);

  switch (type)
  {
    case ::prometheus::MetricType::Counter: {
      ASSERT_DOUBLE_EQ(metric_data.counter.value, vals[0]);
      break;
    }
    case ::prometheus::MetricType::Histogram: {
      ASSERT_DOUBLE_EQ(metric_data.histogram.sample_count, vals[0]);
      ASSERT_DOUBLE_EQ(metric_data.histogram.sample_sum, vals[1]);
      auto buckets = metric_data.histogram.bucket;
      ASSERT_EQ(buckets.size(), vals[2]);
      break;
    }
    case ::prometheus::MetricType::Gauge: {
      ASSERT_DOUBLE_EQ(metric_data.gauge.value, vals[0]);
      break;
    }
    break;
    case ::prometheus::MetricType::Summary:
      // Summary and Info type not supported
      ASSERT_TRUE(false);
      break;
    case prometheus::MetricType::Untyped:
      break;
    default:
      break;
  }
}

void assert_histogram(::prometheus::MetricFamily &metric,
                      std::list<double> boundaries,
                      std::vector<int> correct)
{
  int cumulative_count = 0;
  auto buckets         = metric.metric[0].histogram.bucket;
  auto boundary_it     = boundaries.cbegin();
  for (size_t i = 0; i < buckets.size(); i++)
  {
    auto bucket = buckets[i];
    if (i != buckets.size() - 1)
    {
      ASSERT_DOUBLE_EQ(*boundary_it++, bucket.upper_bound);
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
  metric_sdk::ResourceMetrics metrics_data = {};
  auto translated                          = TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 0);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerCounter)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateSumPointData();

  auto translated = TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 1);

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", ::prometheus::MetricType::Counter, 1,
               vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerLastValue)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateLastValuePointData();

  auto translated = TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 1);

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", ::prometheus::MetricType::Gauge, 1,
               vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramNormal)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateHistogramPointData();

  auto translated = TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 1);

  auto metric              = translated[0];
  std::vector<double> vals = {3, 900.5, 4};
  assert_basic(metric, "library_name", "description", ::prometheus::MetricType::Histogram, 1,
               vals);
  assert_histogram(metric, std::list<double>{10.1, 20.2, 30.2}, {200, 300, 400, 500});
}

class SanitizeNameTest : public ::testing::Test
{
  Resource resource_ = Resource::Create(ResourceAttributes{});
  nostd::unique_ptr<InstrumentationScope> instrumentation_scope_ =
      InstrumentationScope::Create("library_name", "1.2.0");

protected:
  void CheckSanitation(const std::string &original, const std::string &sanitized)
  {
    metric_sdk::InstrumentDescriptor instrument_descriptor{
        original, "description", "unit", metric_sdk::InstrumentType::kCounter,
        metric_sdk::InstrumentValueType::kDouble};
    std::vector<prometheus::MetricFamily> result = TranslateToPrometheus(
        {&resource_,
         {{instrumentation_scope_.get(), {{instrument_descriptor, {}, {}, {}, {{}}}}}}});
    EXPECT_EQ(result.begin()->name, sanitized + "_unit");
  }
};

TEST_F(SanitizeNameTest, SanitizeName)
{
  CheckSanitation("name", "name");
  CheckSanitation("name?", "name_");
  CheckSanitation("name???", "name_");
  CheckSanitation("name?__name", "name_name");
  CheckSanitation("name?__name:", "name_name:");
}

OPENTELEMETRY_END_NAMESPACE
