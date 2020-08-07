/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "opentelemetry/exporters/prometheus/prometheus_exporter_utils.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#include "prometheus/metric_type.h"

#define QUANTILE_STEP 0.25

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
    std::vector<metric_sdk::Record> &records)
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
  try
  {
    auto sanitized      = SanitizeNames(record.GetName());
    metric_family->name = sanitized;
    std::cout << "Sanitized metric name \"" << record.GetName() << "\" to \"" << sanitized << "\""
              << std::endl;
  }
  catch (std::invalid_argument &e)
  {
    metric_family->name = record.GetName();
  }
  metric_family->help = record.GetDescription();

  // unpack the variant and set the metric data to metric family struct
  auto agg_var    = record.GetAggregator();
  auto labels_str = record.GetLabels();
  if (nostd::holds_alternative<nostd::shared_ptr<metric_sdk::Aggregator<int>>>(agg_var))
  {
    auto aggregator = nostd::get<nostd::shared_ptr<metric_sdk::Aggregator<int>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
  else if (nostd::holds_alternative<nostd::shared_ptr<metric_sdk::Aggregator<short>>>(agg_var))
  {
    auto aggregator = nostd::get<nostd::shared_ptr<metric_sdk::Aggregator<short>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
  else if (nostd::holds_alternative<nostd::shared_ptr<metric_sdk::Aggregator<float>>>(
               record.GetAggregator()))
  {
    auto aggregator = nostd::get<nostd::shared_ptr<metric_sdk::Aggregator<float>>>(agg_var);
    SetMetricFamilyByAggregator(aggregator, labels_str, metric_family);
  }
  else if (nostd::holds_alternative<nostd::shared_ptr<metric_sdk::Aggregator<double>>>(
               record.GetAggregator()))
  {
    auto aggregator = nostd::get<nostd::shared_ptr<metric_sdk::Aggregator<double>>>(agg_var);
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
  // name cannot be null or empty.
  // a valid OTel name should only contain alphanumeric characters,
  // '_', '.', or '-'.
  if (!IsValidName(name))
  {
    throw std::invalid_argument("Received an invalid OTel name.\n");
  }

  // replace all '.' and '-' with '_'
  std::replace(name.begin(), name.end(), '.', '_');
  std::replace(name.begin(), name.end(), '-', '_');

  // if the replaced name starts with '_', it's also invalid
  if (name[0] == '_')
  {
    throw std::invalid_argument("Received an invalid OTel name.\n");
  }

  return name;
}

/**
 * Determine whether the input name is a valid OTel name or not
 *
 * From the spec:
 * 1. They are non-empty strings
 * 2. They are case-insensitive
 * 3. The first character must be non-numeric, non-space, non-punctuation
 * 4. Subsequent characters must belong to the alphanumeric characters, '_', '.', and '-'.
 */
bool PrometheusExporterUtils::IsValidName(const std::string &name)
{
  if (name.empty())
  {
    return false;
  }

  if (isnumber(name[0]) || isspace(name[0]) || ispunct(name[0]))
  {
    return false;
  }

  for (std::string::size_type i = 1; i < name.size(); i++)
  {
    if (!isalnum(name[i]) && name[i] != '_' && name[i] != '.' && name[i] != '-')
    {
      return false;
    }
  }

  return true;
}

/**
 * Set value to metric family for different aggregator
 */
template <typename T>
void PrometheusExporterUtils::SetMetricFamilyByAggregator(
    nostd::shared_ptr<metric_sdk::Aggregator<T>> aggregator,
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
    auto quantiles = GetQuantilesVector(aggregator);
    if (kind == metric_sdk::AggregatorKind::Exact)
    {
      // TODO: 1. what if this Exact aggregator is not for quantile estimation?
      // TODO: 2. how many quantile samples should I include in each metric?
      if (aggregator->get_quant_estimation())
      {
        SetData(checkpointed_values, kind, quantiles, labels_str, time, metric_family);
      }
    }
    else if (kind == metric_sdk::AggregatorKind::Sketch)
    {
      SetData(checkpointed_values, kind, quantiles, labels_str, time, metric_family);
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
                                      prometheus_client::MetricFamily *metric_family)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, time, labels);
  SetValue(values, kind, quantiles, &metric);
}

/**
 * Set time and labels to metric data
 */
void PrometheusExporterUtils::SetMetricBasic(prometheus_client::ClientMetric &metric,
                                             std::chrono::nanoseconds time,
                                             const std::string &labels)
{
  metric.timestamp_ms = time.count() / 1000;

  auto label_pairs = ParseLabel(labels);
  if (!label_pairs.empty())
  {
    metric.label.resize(label_pairs.size());
    for (int i = 0; i < label_pairs.size(); ++i)
    {
      try
      {
        auto sanitized       = SanitizeNames(label_pairs[i].first);
        metric.label[i].name = sanitized;
        std::cout << "Sanitized label name \"" << label_pairs[i].first << "\" to \"" << sanitized
                  << "\"" << std::endl;
      }
      catch (std::invalid_argument &e)
      {
        metric.label[i].name = label_pairs[i].first;
      }
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
    nostd::shared_ptr<metric_sdk::Aggregator<T>> aggregator)
{
  std::vector<T> quantiles;
  for (double q = 0; q <= 1; q += QUANTILE_STEP)
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
    case prometheus_client::MetricType::Counter:
    {
      metric->counter.value = values[0];
      break;
    }
    case prometheus_client::MetricType::Gauge:
    {
      metric->gauge.value = values[0];
      break;
    }
    case prometheus_client::MetricType::Untyped:
    {
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
  for (int i = 0; i < boundaries.size() + 1; i++)
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
                                       prometheus_client::ClientMetric *metric)
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

  double quant = 0;
  std::vector<prometheus_client::ClientMetric::Quantile> prometheus_quantiles;
  for (int i = 0; i < quantiles.size(); i++)
  {
    prometheus_client::ClientMetric::Quantile quantile;
    quantile.quantile = quant;
    quantile.value    = quantiles[i];
    quant += QUANTILE_STEP;
    prometheus_quantiles.emplace_back(quantile);
  }
  metric->summary.quantile = prometheus_quantiles;
}
}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
