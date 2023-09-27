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
      // Summary and Info type not supported
      ASSERT_TRUE(false);
      break;
    case prometheus::MetricType::Untyped:
      break;
    default:
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
  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 0);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerCounter)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateSumPointData();

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 1);

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", prometheus_client::MetricType::Counter, 1,
               vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerLastValue)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateLastValuePointData();

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 1);

  auto metric1          = translated[0];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", prometheus_client::MetricType::Gauge, 1,
               vals);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramNormal)
{
  TestDataPoints dp;
  metric_sdk::ResourceMetrics metrics_data = dp.CreateHistogramPointData();

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 1);

  auto metric              = translated[0];
  std::vector<double> vals = {3, 900.5, 4};
  assert_basic(metric, "library_name", "description", prometheus_client::MetricType::Histogram, 1,
               vals);
  assert_histogram(metric, std::list<double>{10.1, 20.2, 30.2}, {200, 300, 400, 500});
}

class SanitizeNameTest : public ::testing::Test
{
  Resource resource_ = Resource::Create({});
  nostd::unique_ptr<InstrumentationScope> instrumentation_scope_ =
      InstrumentationScope::Create("library_name", "1.2.0");

protected:
  void CheckSanitation(const std::string &original, const std::string &sanitized)
  {
    metric_sdk::InstrumentDescriptor instrument_descriptor_{
        original, "description", "unit", metric_sdk::InstrumentType::kCounter,
        metric_sdk::InstrumentValueType::kDouble};
    std::vector<prometheus::MetricFamily> result = PrometheusExporterUtils::TranslateToPrometheus(
        {&resource_,
         {{instrumentation_scope_.get(), {{instrument_descriptor_, {}, {}, {}, {{{}, {}}}}}}}});
    EXPECT_EQ(result.begin()->name, sanitized + "_unit");
  }
};

TEST_F(SanitizeNameTest, All)
{
  CheckSanitation("name", "name");
  CheckSanitation("name?", "name_");
  CheckSanitation("name???", "name_");
  CheckSanitation("name?__", "name_");
  CheckSanitation("name?__name", "name_name");
  CheckSanitation("name?__name:", "name_name:");
}

class AttributeCollisionTest : public ::testing::Test
{
  Resource resource_ = Resource::Create(ResourceAttributes{});
  nostd::unique_ptr<InstrumentationScope> instrumentation_scope_ =
      InstrumentationScope::Create("library_name", "1.2.0");
  metric_sdk::InstrumentDescriptor instrument_descriptor_{"library_name", "description", "unit",
                                                          metric_sdk::InstrumentType::kCounter,
                                                          metric_sdk::InstrumentValueType::kDouble};

protected:
  void CheckTranslation(const metric_sdk::PointAttributes &attrs,
                        const std::vector<prometheus::ClientMetric::Label> &expected)
  {
    std::vector<prometheus::MetricFamily> result = PrometheusExporterUtils::TranslateToPrometheus(
        {&resource_,
         {{instrumentation_scope_.get(), {{instrument_descriptor_, {}, {}, {}, {{attrs, {}}}}}}}});
    EXPECT_EQ(result.begin()->metric.begin()->label, expected);
  }
};

TEST_F(AttributeCollisionTest, SeparatesDistinctKeys)
{
  CheckTranslation({{"foo.a", "value1"}, {"foo.b", "value2"}},
                   {{"foo_a", "value1"}, {"foo_b", "value2"}});
}

TEST_F(AttributeCollisionTest, JoinsCollidingKeys)
{
  CheckTranslation({{"foo.a", "value1"}, {"foo_a", "value2"}},  //
                   {{"foo_a", "value1;value2"}});
}

TEST_F(AttributeCollisionTest, DropsInvertedKeys)
{
  CheckTranslation({{"foo.a", "value1"}, {"foo.b", "value2"}, {"foo__a", "value3"}},
                   {{"foo_a", "value1"}, {"foo_b", "value2"}});
}

OPENTELEMETRY_END_NAMESPACE
