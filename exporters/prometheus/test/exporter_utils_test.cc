// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

#include "opentelemetry/exporters/prometheus/exporter_utils.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "prometheus_test_helper.h"

using opentelemetry::exporter::metrics::PrometheusExporterUtils;
namespace metric_sdk        = opentelemetry::sdk::metrics;
namespace metric_api        = opentelemetry::metrics;
namespace prometheus_client = ::prometheus;

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace metrics
{
class SanitizeNameTester
{
public:
  static std::string sanitize(std::string name)
  {
    return PrometheusExporterUtils::SanitizeNames(name);
  }
};
}  // namespace metrics
}  // namespace exporter

template <typename T>
void assert_basic(prometheus_client::MetricFamily &metric,
                  const std::string &sanitized_name,
                  const std::string &description,
                  prometheus_client::MetricType type,
                  size_t label_num,
                  std::vector<T> vals)
{
  ASSERT_EQ(metric.name, sanitized_name + "_unit");  // name sanitized
  ASSERT_EQ(metric.help, description);               // description not changed
  ASSERT_EQ(metric.type, type);                      // type translated

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

  opentelemetry::sdk::resource::Resource resource = opentelemetry::sdk::resource::Resource::Create(
      {{"service.name", "test_service"},
       {"service.namespace", "test_namespace"},
       {"service.instance.id", "localhost:8000"},
       {"custom_resource_attr", "custom_resource_value"}});
  metric_sdk::ResourceMetrics metrics_data = CreateSumPointData();
  metrics_data.resource_                   = &resource;

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 2);

  auto metric1          = translated[1];
  std::vector<int> vals = {10};

  assert_basic(metric1, "library_name", "description", prometheus_client::MetricType::Counter, 3,
               vals);

  int checked_label_num = 0;
  for (auto &label : metric1.metric[0].label)
  {
    if (label.name == "job")
    {
      ASSERT_EQ(label.value, "test_namespace/test_service");
      checked_label_num++;
    }
    else if (label.name == "instance")
    {
      ASSERT_EQ(label.value, "localhost:8000");
      checked_label_num++;
    }
  }
  ASSERT_EQ(checked_label_num, 2);

  checked_label_num = 0;
  for (auto &label : translated[0].metric[0].label)
  {
    if (label.name == "job")
    {
      ASSERT_EQ(label.value, "test_namespace/test_service");
      checked_label_num++;
    }
    else if (label.name == "instance")
    {
      ASSERT_EQ(label.value, "localhost:8000");
      checked_label_num++;
    }
    else if (label.name == "custom_resource_attr")
    {
      ASSERT_EQ(label.value, "custom_resource_value");
      checked_label_num++;
    }
  }
  ASSERT_EQ(checked_label_num, 3);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusIntegerLastValue)
{
  opentelemetry::sdk::resource::Resource resource = opentelemetry::sdk::resource::Resource::Create(
      {{"service.name", "test_service"},
       {"service.instance.id", "localhost:8000"},
       {"custom_resource_attr", "custom_resource_value"}});
  metric_sdk::ResourceMetrics metrics_data = CreateLastValuePointData();
  metrics_data.resource_                   = &resource;

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 2);

  auto metric1          = translated[1];
  std::vector<int> vals = {10};
  assert_basic(metric1, "library_name", "description", prometheus_client::MetricType::Gauge, 3,
               vals);

  int checked_label_num = 0;
  for (auto &label : metric1.metric[0].label)
  {
    if (label.name == "job")
    {
      ASSERT_EQ(label.value, "test_service");
      checked_label_num++;
    }
    else if (label.name == "instance")
    {
      ASSERT_EQ(label.value, "localhost:8000");
      checked_label_num++;
    }
  }
  ASSERT_EQ(checked_label_num, 2);

  checked_label_num = 0;
  for (auto &label : translated[0].metric[0].label)
  {
    if (label.name == "job")
    {
      ASSERT_EQ(label.value, "test_service");
      checked_label_num++;
    }
    else if (label.name == "instance")
    {
      ASSERT_EQ(label.value, "localhost:8000");
      checked_label_num++;
    }
    else if (label.name == "custom_resource_attr")
    {
      ASSERT_EQ(label.value, "custom_resource_value");
      checked_label_num++;
    }
  }
  ASSERT_EQ(checked_label_num, 3);
}

TEST(PrometheusExporterUtils, TranslateToPrometheusHistogramNormal)
{
  opentelemetry::sdk::resource::Resource resource = opentelemetry::sdk::resource::Resource::Create(
      {{"job", "test_service2"},
       {"instance", "localhost:8001"},
       {"custom_resource_attr", "custom_resource_value"}});
  metric_sdk::ResourceMetrics metrics_data = CreateHistogramPointData();
  metrics_data.resource_                   = &resource;

  auto translated = PrometheusExporterUtils::TranslateToPrometheus(metrics_data);
  ASSERT_EQ(translated.size(), 2);

  auto metric              = translated[1];
  std::vector<double> vals = {3, 900.5, 4};
  assert_basic(metric, "library_name", "description", prometheus_client::MetricType::Histogram, 3,
               vals);
  assert_histogram(metric, std::list<double>{10.1, 20.2, 30.2}, {200, 300, 400, 500});

  int checked_label_num = 0;
  for (auto &label : metric.metric[0].label)
  {
    if (label.name == "job")
    {
      // job label should be ignored when service.name exist
      ASSERT_EQ(label.value, "unknown_service");
      checked_label_num++;
    }
    else if (label.name == "instance")
    {
      ASSERT_EQ(label.value, "localhost:8001");
      checked_label_num++;
    }
  }
  ASSERT_EQ(checked_label_num, 2);

  checked_label_num = 0;
  for (auto &label : translated[0].metric[0].label)
  {
    if (label.name == "job")
    {
      // job label should be ignored when service.name exist
      ASSERT_EQ(label.value, "unknown_service");
      checked_label_num++;
    }
    else if (label.name == "instance")
    {
      ASSERT_EQ(label.value, "localhost:8001");
      checked_label_num++;
    }
    else if (label.name == "custom_resource_attr")
    {
      ASSERT_EQ(label.value, "custom_resource_value");
      checked_label_num++;
    }
  }
  ASSERT_EQ(checked_label_num, 3);
}

TEST(PrometheusExporterUtils, SanitizeName)
{
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::sanitize("name"), "name");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::sanitize("name?"), "name_");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::sanitize("name???"), "name_");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::sanitize("name?__"), "name_");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::sanitize("name?__name"), "name_name");
  ASSERT_EQ(exporter::metrics::SanitizeNameTester::sanitize("name?__name:"), "name_name:");
}

OPENTELEMETRY_END_NAMESPACE
