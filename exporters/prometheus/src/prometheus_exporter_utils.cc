// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <iostream>
#  include <sstream>
#  include <utility>
#  include <vector>

#  include "opentelemetry/exporters/prometheus/prometheus_exporter_utils.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"
#  include "prometheus/metric_type.h"

namespace prometheus_client = ::prometheus;
namespace metric_sdk        = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace prometheus
{
/**
 * Helper function to convert OpenTelemetry metrics data collection
 * to Prometheus metrics data collection
 *
 * @param records a collection of metrics in OpenTelemetry
 * @return a collection of translated metrics that is acceptable by Prometheus
 */
std::vector<prometheus_client::MetricFamily> PrometheusExporterUtils::TranslateToPrometheus(
    const std::vector<metric_sdk::Record> &records)
{
  if (records.empty())
  {
    return {};
  }

  // initialize output vector
  std::vector<prometheus_client::MetricFamily> output(records.size());

  // iterate through the vector and set result data into it
  int i = 0;
  for (auto r : records)
  {
    SetMetricFamily(r, &output[i]);
    i++;
  }

  return output;
}

// ======================= private helper functions =========================
/**
 * Set value to metric family according to record
 */
void PrometheusExporterUtils::SetMetricFamily(metric_sdk::Record &record,
                                              prometheus_client::MetricFamily *metric_family)
{

  auto origin_name    = record.GetName();
  auto sanitized      = SanitizeNames(origin_name);
  metric_family->name = sanitized;
  metric_family->help = record.GetDescription();

  // unpack the variant and set the metric data to metric family struct
  auto agg_var    = record.GetAggregator();
  auto labels_str = record.GetLabels();
  if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<int>>>(agg_var))
  {
    auto aggregator = nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
  else if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<short>>>(agg_var))
  {
    auto aggregator = nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
  else if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<float>>>(
               record.GetAggregator()))
  {
    auto aggregator = nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
  else if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<double>>>(
               record.GetAggregator()))
  {
    auto aggregator = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
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
template <typename T>
void PrometheusExporterUtils::SetMetricFamilyByAggregator(
    std::shared_ptr<metric_sdk::Aggregator<T>> aggregator,
    std::string labels_str,
    prometheus_client::MetricFamily *metric_family)
{
  // get aggregator kind and translate to Prometheus metric type
  auto kind                                = aggregator->get_aggregator_kind();
  const prometheus_client::MetricType type = TranslateType(kind);
  metric_family->type                      = type;
  // get check-pointed values, label string and check-pointed time
  auto checkpointed_values = aggregator->get_checkpoint();
  auto time                = aggregator->get_checkpoint_timestamp().time_since_epoch();

  if (type == prometheus_client::MetricType::Histogram)  // Histogram
  {
    auto boundaries = aggregator->get_boundaries();
    auto counts     = aggregator->get_counts();
    SetData(checkpointed_values, boundaries, counts, labels_str, time, metric_family);
  }
  else if (type == prometheus_client::MetricType::Summary)  // Sketch, Exact
  {
    std::vector<double> quantile_points = {0, 0.5, 0.9, 0.95, 0.99, 1};
    if (kind == metric_sdk::AggregatorKind::Exact)
    {
      std::vector<T> quantiles;
      bool do_quantile = aggregator->get_quant_estimation();
      if (do_quantile)
      {
        quantiles = GetQuantilesVector(aggregator, quantile_points);
      }
      SetData(checkpointed_values, kind, quantiles, labels_str, time, metric_family, do_quantile,
              quantile_points);
    }
    else if (kind == metric_sdk::AggregatorKind::Sketch)
    {
      auto quantiles = GetQuantilesVector(aggregator, quantile_points);
      SetData(checkpointed_values, kind, quantiles, labels_str, time, metric_family, true,
              quantile_points);
    }
  }
  else  // Counter, Gauge, MinMaxSumCount, Untyped
  {
    // Handle MinMaxSumCount: https://github.com/open-telemetry/opentelemetry-cpp/issues/228
    // Use sum/count is ok.
    if (kind == metric_sdk::AggregatorKind::MinMaxSumCount)
    {
      double avg = (double)checkpointed_values[2] / checkpointed_values[3];
      SetData(avg, labels_str, time, metric_family);
    }
    else
    {
      SetData(checkpointed_values, labels_str, type, time, metric_family);
    }
  }
}

/**
 * Translate the OTel metric type to Prometheus metric type
 */
prometheus_client::MetricType PrometheusExporterUtils::TranslateType(
    metric_sdk::AggregatorKind kind)
{
  switch (kind)
  {
    case metric_sdk::AggregatorKind::Counter:
      return prometheus_client::MetricType::Counter;
    case metric_sdk::AggregatorKind::Gauge:
    case metric_sdk::AggregatorKind::MinMaxSumCount:
      return prometheus_client::MetricType::Gauge;
    case metric_sdk::AggregatorKind::Histogram:
      return prometheus_client::MetricType::Histogram;
    case metric_sdk::AggregatorKind::Sketch:
    case metric_sdk::AggregatorKind::Exact:
      return prometheus_client::MetricType::Summary;
    default:
      return prometheus_client::MetricType::Untyped;
  }
}

/**
 * Set metric data for:
 * Counter => Prometheus Counter
 * Gauge => Prometheus Gauge
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
                                      const std::vector<double> &boundaries,
                                      const std::vector<int> &counts,
                                      const std::string &labels,
                                      std::chrono::nanoseconds time,
                                      prometheus_client::MetricFamily *metric_family)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, time, labels);
  SetValue(values, boundaries, counts, &metric);
}

/**
 * Set metric data for:
 * MinMaxSumCount => Prometheus Gauge
 * Use Average (sum / count) as the gauge metric
 */
void PrometheusExporterUtils::SetData(double value,
                                      const std::string &labels,
                                      std::chrono::nanoseconds time,
                                      prometheus_client::MetricFamily *metric_family)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, time, labels);
  SetValue(value, &metric);
}

/**
 * Set metric data for:
 * Exact => Prometheus Summary
 * Sketch => Prometheus Summary
 */
template <typename T>
void PrometheusExporterUtils::SetData(std::vector<T> values,
                                      metric_sdk::AggregatorKind kind,
                                      const std::vector<T> &quantiles,
                                      const std::string &labels,
                                      std::chrono::nanoseconds time,
                                      prometheus_client::MetricFamily *metric_family,
                                      bool do_quantile,
                                      std::vector<double> quantile_points)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, time, labels);
  SetValue(values, kind, quantiles, &metric, do_quantile, quantile_points);
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
 * Build a quantiles vector from aggregator
 */
template <typename T>
std::vector<T> PrometheusExporterUtils::GetQuantilesVector(
    std::shared_ptr<metric_sdk::Aggregator<T>> aggregator,
    const std::vector<double> &quantile_points)
{
  std::vector<T> quantiles;
  for (double q : quantile_points)
  {
    T quantile = aggregator->get_quantiles(q);
    quantiles.emplace_back(quantile);
  }
  return quantiles;
}

/**
 * Handle Counter and Gauge.
 */
template <typename T>
void PrometheusExporterUtils::SetValue(std::vector<T> values,
                                       prometheus_client::MetricType type,
                                       prometheus_client::ClientMetric *metric)
{
  switch (type)
  {
    case prometheus_client::MetricType::Counter: {
      metric->counter.value = values[0];
      break;
    }
    case prometheus_client::MetricType::Gauge: {
      metric->gauge.value = values[0];
      break;
    }
    case prometheus_client::MetricType::Untyped: {
      metric->untyped.value = values[0];
      break;
    }
    default:
      return;
  }
}

/**
 * Handle Gauge from MinMaxSumCount
 */
void PrometheusExporterUtils::SetValue(double value, prometheus_client::ClientMetric *metric)
{
  metric->gauge.value = value;
}

/**
 * Handle Histogram
 */
template <typename T>
void PrometheusExporterUtils::SetValue(std::vector<T> values,
                                       std::vector<double> boundaries,
                                       std::vector<int> counts,
                                       prometheus_client::ClientMetric *metric)
{
  metric->histogram.sample_sum   = values[0];
  metric->histogram.sample_count = values[1];
  int cumulative                 = 0;
  std::vector<prometheus_client::ClientMetric::Bucket> buckets;
  for (size_t i = 0; i < boundaries.size() + 1; i++)
  {
    prometheus_client::ClientMetric::Bucket bucket;
    cumulative += counts[i];
    bucket.cumulative_count = cumulative;
    if (i != boundaries.size())
    {
      bucket.upper_bound = boundaries[i];
    }
    else
    {
      bucket.upper_bound = std::numeric_limits<double>::infinity();
    }
    buckets.emplace_back(bucket);
  }
  metric->histogram.bucket = buckets;
}

/**
 * Handle Exact and Sketch
 */
template <typename T>
void PrometheusExporterUtils::SetValue(std::vector<T> values,
                                       metric_sdk::AggregatorKind kind,
                                       std::vector<T> quantiles,
                                       prometheus_client::ClientMetric *metric,
                                       bool do_quantile,
                                       const std::vector<double> &quantile_points)
{
  if (kind == metric_sdk::AggregatorKind::Exact)
  {
    metric->summary.sample_count = values.size();
    auto sum                     = 0;
    for (auto val : values)
    {
      sum += val;
    }
    metric->summary.sample_sum = sum;
  }
  else if (kind == metric_sdk::AggregatorKind::Sketch)
  {
    metric->summary.sample_sum   = values[0];
    metric->summary.sample_count = values[1];
  }

  if (do_quantile)
  {
    std::vector<prometheus_client::ClientMetric::Quantile> prometheus_quantiles;
    for (size_t i = 0; i < quantiles.size(); i++)
    {
      prometheus_client::ClientMetric::Quantile quantile;
      quantile.quantile = quantile_points[i];
      quantile.value    = quantiles[i];
      prometheus_quantiles.emplace_back(quantile);
    }
    metric->summary.quantile = prometheus_quantiles;
  }
}
}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
