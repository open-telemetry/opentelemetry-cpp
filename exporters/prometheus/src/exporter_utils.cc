// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include <iostream>
#  include <sstream>
#  include <utility>
#  include <vector>

#  include "opentelemetry/exporters/prometheus/exporter_utils.h"
#  include "opentelemetry/sdk/metrics/export/metric_producer.h"
#  include "prometheus/metric_type.h"

namespace prometheus_client = ::prometheus;
namespace metric_sdk        = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
/**
 * Helper function to convert OpenTelemetry metrics data collection
 * to Prometheus metrics data collection
 *
 * @param records a collection of metrics in OpenTelemetry
 * @return a collection of translated metrics that is acceptable by Prometheus
 */
std::vector<prometheus_client::MetricFamily> PrometheusExporterUtils::TranslateToPrometheus(
    const std::vector<std::unique_ptr<sdk::metrics::ResourceMetrics>> &data)
{
  if (data.empty())
  {
    return {};
  }

  // initialize output vector
  std::vector<prometheus_client::MetricFamily> output(data.size());

  // iterate through the vector and set result data into it
  int i = 0;
  for (const auto &r : data)
  {
    SetMetricFamily(*r, &output[i]);
    i++;
  }

  return output;
}

/**
 * Set value to metric family according to record
 */
void PrometheusExporterUtils::SetMetricFamily(sdk::metrics::ResourceMetrics &data,
                                              prometheus_client::MetricFamily *metric_family)
{
  SetMetricFamilyByAggregator(data, "", metric_family);
}

/**
 * Sanitize the given metric name or label according to Prometheus rule.
 *
 * This function is needed because names in OpenTelemetry can contain
 * alphanumeric characters, '_', '.', and '-', whereas in Prometheus the
 * name should only contain alphanumeric characters and '_'.
 */
std::string PrometheusExporterUtils::SanitizeNames(std::string name)
{
  // replace all '.' and '-' with '_'
  std::replace(name.begin(), name.end(), '.', '_');
  std::replace(name.begin(), name.end(), '-', '_');

  return name;
}

/**
 * Set value to metric family for different aggregator
 */
void PrometheusExporterUtils::SetMetricFamilyByAggregator(
    const sdk::metrics::ResourceMetrics &data,
    std::string labels_str,
    prometheus_client::MetricFamily *metric_family)
{
  for (const auto &instrumentation_info : data.instrumentation_info_metric_data_)
  {
    auto origin_name    = instrumentation_info.instrumentation_library_->GetName();
    auto sanitized      = SanitizeNames(origin_name);
    metric_family->name = sanitized;
    for (const auto &metric_data : instrumentation_info.metric_data_)
    {
      auto time = metric_data.start_ts.time_since_epoch();
      for (const auto &point_data_attr : metric_data.point_data_attr_)
      {
        auto kind                                = getAggregationType(point_data_attr.point_data);
        const prometheus_client::MetricType type = TranslateType(kind);
        metric_family->type                      = type;
        if (type == prometheus_client::MetricType::Histogram)  // Histogram
        {
          auto histogram_point_data =
              nostd::get<sdk::metrics::HistogramPointData>(point_data_attr.point_data);
          auto boundaries = histogram_point_data.boundaries_;
          auto counts     = histogram_point_data.counts_;
          SetData(std::vector<double>{nostd::get<double>(histogram_point_data.sum_),
                                      (double)histogram_point_data.count_},
                  boundaries, counts, labels_str, time, metric_family);
        }
        else  // Counter, Untyped
        {
          auto sum_point_data = nostd::get<sdk::metrics::SumPointData>(point_data_attr.point_data);
          std::vector<metric_sdk::ValueType> values{sum_point_data.value_};
          SetData(values, labels_str, type, time, metric_family);
        }
      }
    }
  }
}

metric_sdk::AggregationType PrometheusExporterUtils::getAggregationType(
    const metric_sdk::PointType &point_type)
{
  if (nostd::holds_alternative<sdk::metrics::SumPointData>(point_type))
  {
    return metric_sdk::AggregationType::kSum;
  }
  else if (nostd::holds_alternative<sdk::metrics::DropPointData>(point_type))
  {
    return metric_sdk::AggregationType::kDrop;
  }
  else if (nostd::holds_alternative<sdk::metrics::HistogramPointData>(point_type))
  {
    return metric_sdk::AggregationType::kHistogram;
  }
  else if (nostd::holds_alternative<sdk::metrics::LastValuePointData>(point_type))
  {
    return metric_sdk::AggregationType::kLastValue;
  }
  return metric_sdk::AggregationType::kDefault;
}

/**
 * Translate the OTel metric type to Prometheus metric type
 */
prometheus_client::MetricType PrometheusExporterUtils::TranslateType(
    metric_sdk::AggregationType kind)
{
  switch (kind)
  {
    case metric_sdk::AggregationType::kSum:
      return prometheus_client::MetricType::Counter;
    case metric_sdk::AggregationType::kHistogram:
      return prometheus_client::MetricType::Histogram;
    default:
      return prometheus_client::MetricType::Untyped;
  }
}

/**
 * Set metric data for:
 * sum => Prometheus Counter
 */
template <typename T>
void PrometheusExporterUtils::SetData(std::vector<T> values,
                                      const std::string &labels,
                                      prometheus_client::MetricType type,
                                      std::chrono::nanoseconds time,
                                      prometheus_client::MetricFamily *metric_family)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, time, labels);
  SetValue(values, type, &metric);
}

/**
 * Set metric data for:
 * Histogram => Prometheus Histogram
 */
template <typename T>
void PrometheusExporterUtils::SetData(std::vector<T> values,
                                      const opentelemetry::sdk::metrics::ListType &boundaries,
                                      const std::vector<uint64_t> &counts,
                                      const std::string &labels,
                                      std::chrono::nanoseconds time,
                                      prometheus_client::MetricFamily *metric_family)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, time, labels);
  if (nostd::holds_alternative<std::list<long>>(boundaries))
  {
    SetValue(values, nostd::get<std::list<long>>(boundaries), counts, &metric);
  }
  else
  {
    SetValue(values, nostd::get<std::list<double>>(boundaries), counts, &metric);
  }
}

/**
 * Set time and labels to metric data
 */
void PrometheusExporterUtils::SetMetricBasic(prometheus_client::ClientMetric &metric,
                                             std::chrono::nanoseconds time,
                                             const std::string &labels)
{
  metric.timestamp_ms = time.count() / 1000000;

  auto label_pairs = ParseLabel(labels);
  if (!label_pairs.empty())
  {
    metric.label.resize(label_pairs.size());
    for (size_t i = 0; i < label_pairs.size(); ++i)
    {
      auto origin_name      = label_pairs[i].first;
      auto sanitized        = SanitizeNames(origin_name);
      metric.label[i].name  = sanitized;
      metric.label[i].value = label_pairs[i].second;
    }
  }
};

/**
 * Parse a string of labels (key:value) into a vector of pairs
 * {,}
 * {l1:v1,l2:v2,...,}
 */
std::vector<std::pair<std::string, std::string>> PrometheusExporterUtils::ParseLabel(
    std::string labels)
{
  if (labels.size() < 3)
  {
    return {};
  }
  labels = labels.substr(1, labels.size() - 2);

  std::vector<std::string> paired_labels;
  std::stringstream s_stream(labels);
  while (s_stream.good())
  {
    std::string substr;
    getline(s_stream, substr, ',');  // get first string delimited by comma
    if (!substr.empty())
    {
      paired_labels.push_back(substr);
    }
  }

  std::vector<std::pair<std::string, std::string>> result;
  for (auto &paired : paired_labels)
  {
    std::size_t split_index = paired.find(':');
    std::string label       = paired.substr(0, split_index);
    std::string value       = paired.substr(split_index + 1);
    result.emplace_back(std::pair<std::string, std::string>(label, value));
  }

  return result;
}

/**
 * Handle Counter.
 */
template <typename T>
void PrometheusExporterUtils::SetValue(std::vector<T> values,
                                       prometheus_client::MetricType type,
                                       prometheus_client::ClientMetric *metric)
{
  double value          = 0.0;
  const auto &value_var = values[0];
  if (nostd::holds_alternative<long>(value_var))
  {
    value = nostd::get<long>(value_var);
  }
  else
  {
    value = nostd::get<double>(value_var);
  }

  switch (type)
  {
    case prometheus_client::MetricType::Counter: {
      metric->counter.value = value;
      break;
    }
    case prometheus_client::MetricType::Untyped: {
      metric->untyped.value = value;
      break;
    }
    default:
      return;
  }
}

/**
 * Handle Histogram
 */
template <typename T, typename U>
void PrometheusExporterUtils::SetValue(std::vector<T> values,
                                       const std::list<U> &boundaries,
                                       const std::vector<uint64_t> &counts,
                                       prometheus_client::ClientMetric *metric)
{
  metric->histogram.sample_sum   = values[0];
  metric->histogram.sample_count = values[1];
  int cumulative                 = 0;
  std::vector<prometheus_client::ClientMetric::Bucket> buckets;
  uint32_t idx = 0;
  for (const auto &boundary : boundaries)
  {
    prometheus_client::ClientMetric::Bucket bucket;
    cumulative += counts[idx];
    bucket.cumulative_count = cumulative;
    bucket.upper_bound      = boundary;
    buckets.emplace_back(bucket);
    ++idx;
  }
  prometheus_client::ClientMetric::Bucket bucket;
  cumulative += counts[idx];
  bucket.cumulative_count = cumulative;
  bucket.upper_bound      = std::numeric_limits<double>::infinity();
  buckets.emplace_back(bucket);
  metric->histogram.bucket = buckets;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
