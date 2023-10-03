// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "prometheus/metric_family.h"
#include "prometheus/metric_type.h"

#include "opentelemetry/exporters/prometheus/exporter_utils.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/resource/semantic_conventions.h"
#include "opentelemetry/trace/semantic_conventions.h"

#include "opentelemetry/sdk/common/global_log_handler.h"

namespace prometheus_client = ::prometheus;
namespace metric_sdk        = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
namespace
{

static constexpr const char *kScopeNameKey    = "otel_scope_name";
static constexpr const char *kScopeVersionKey = "otel_scope_version";

/**
 * Sanitize the given metric name by replacing invalid characters with _,
 * ensuring that multiple consecutive _ characters are collapsed to a single _.
 *
 * @param valid a callable with the signature `(int pos, char ch) -> bool` that
 *        returns whether `ch` is valid at position `pos` in the string
 * @param name the string to sanitize
 */
template <typename T>
inline std::string Sanitize(std::string name, const T &valid)
{
  static_assert(std::is_convertible<T, std::function<bool(int, char)>>::value,
                "valid should be a callable with the signature "
                "(int, char) -> bool");

  constexpr const auto replacement     = '_';
  constexpr const auto replacement_dup = '=';

  bool has_dup = false;
  for (int i = 0; i < (int)name.size(); ++i)
  {
    if (valid(i, name[i]) && name[i] != replacement)
    {
      continue;
    }
    if (i > 0 && (name[i - 1] == replacement || name[i - 1] == replacement_dup))
    {
      has_dup = true;
      name[i] = replacement_dup;
    }
    else
    {
      name[i] = replacement;
    }
  }
  if (has_dup)
  {
    auto end = std::remove(name.begin(), name.end(), replacement_dup);
    return std::string{name.begin(), end};
  }
  return name;
}

/**
 * Sanitize the given metric label key according to Prometheus rule.
 * Prometheus metric label keys are required to match the following regex:
 *   [a-zA-Z_]([a-zA-Z0-9_])*
 * and multiple consecutive _ characters must be collapsed to a single _.
 */
std::string SanitizeLabel(std::string label_key)
{
  return Sanitize(label_key, [](int i, char c) {
    return (c >= 'a' && c <= 'z') ||  //
           (c >= 'A' && c <= 'Z') ||  //
           c == '_' ||                //
           (c >= '0' && c <= '9' && i > 0);
  });
}

/**
 * Sanitize the given metric name according to Prometheus rule.
 * Prometheus metric names are required to match the following regex:
 *   [a-zA-Z_:]([a-zA-Z0-9_:])*
 * and multiple consecutive _ characters must be collapsed to a single _.
 */
std::string SanitizeName(std::string name)
{
  return Sanitize(name, [](int i, char c) {
    return (c >= 'a' && c <= 'z') ||  //
           (c >= 'A' && c <= 'Z') ||  //
           c == '_' ||                //
           c == ':' ||                //
           (c >= '0' && c <= '9' && i > 0);
  });
}
}  // namespace

/**
 * Helper function to convert OpenTelemetry metrics data collection
 * to Prometheus metrics data collection
 *
 * @param records a collection of metrics in OpenTelemetry
 * @return a collection of translated metrics that is acceptable by Prometheus
 */
std::vector<prometheus_client::MetricFamily> PrometheusExporterUtils::TranslateToPrometheus(
    const sdk::metrics::ResourceMetrics &data,
    bool populate_target_info)
{

  // initialize output vector
  std::size_t reserve_size = 1;
  for (const auto &instrumentation_info : data.scope_metric_data_)
  {
    reserve_size += instrumentation_info.metric_data_.size();
  }

  std::vector<prometheus_client::MetricFamily> output;
  output.reserve(reserve_size);

  // Append target_info as the first metric
  if (populate_target_info && !data.scope_metric_data_.empty())
  {
    SetTarget(data, (*data.scope_metric_data_.begin()).scope_, &output);
  }

  for (const auto &instrumentation_info : data.scope_metric_data_)
  {
    for (const auto &metric_data : instrumentation_info.metric_data_)
    {
      auto origin_name = metric_data.instrument_descriptor.name_;
      auto unit        = metric_data.instrument_descriptor.unit_;
      auto sanitized   = SanitizeName(origin_name);
      prometheus_client::MetricFamily metric_family;
      metric_family.name = sanitized + "_" + unit;
      metric_family.help = metric_data.instrument_descriptor.description_;
      for (const auto &point_data_attr : metric_data.point_data_attr_)
      {
        auto kind         = getAggregationType(point_data_attr.point_data);
        bool is_monotonic = true;
        if (kind == sdk::metrics::AggregationType::kSum)
        {
          is_monotonic =
              nostd::get<sdk::metrics::SumPointData>(point_data_attr.point_data).is_monotonic_;
        }
        const prometheus_client::MetricType type = TranslateType(kind, is_monotonic);
        metric_family.type                       = type;
        if (type == prometheus_client::MetricType::Histogram)  // Histogram
        {
          auto histogram_point_data =
              nostd::get<sdk::metrics::HistogramPointData>(point_data_attr.point_data);
          auto boundaries = histogram_point_data.boundaries_;
          auto counts     = histogram_point_data.counts_;
          double sum      = 0.0;
          if (nostd::holds_alternative<double>(histogram_point_data.sum_))
          {
            sum = nostd::get<double>(histogram_point_data.sum_);
          }
          else
          {
            sum = static_cast<double>(nostd::get<int64_t>(histogram_point_data.sum_));
          }
          SetData(std::vector<double>{sum, (double)histogram_point_data.count_}, boundaries, counts,
                  point_data_attr.attributes, instrumentation_info.scope_, &metric_family,
                  data.resource_);
        }
        else if (type == prometheus_client::MetricType::Gauge)
        {
          if (nostd::holds_alternative<sdk::metrics::LastValuePointData>(
                  point_data_attr.point_data))
          {
            auto last_value_point_data =
                nostd::get<sdk::metrics::LastValuePointData>(point_data_attr.point_data);
            std::vector<metric_sdk::ValueType> values{last_value_point_data.value_};
            SetData(values, point_data_attr.attributes, instrumentation_info.scope_, type,
                    &metric_family, data.resource_);
          }
          else if (nostd::holds_alternative<sdk::metrics::SumPointData>(point_data_attr.point_data))
          {
            auto sum_point_data =
                nostd::get<sdk::metrics::SumPointData>(point_data_attr.point_data);
            std::vector<metric_sdk::ValueType> values{sum_point_data.value_};
            SetData(values, point_data_attr.attributes, instrumentation_info.scope_, type,
                    &metric_family, data.resource_);
          }
          else
          {
            OTEL_INTERNAL_LOG_WARN(
                "[Prometheus Exporter] TranslateToPrometheus - "
                "invalid LastValuePointData type");
          }
        }
        else  // Counter, Untyped
        {
          if (nostd::holds_alternative<sdk::metrics::SumPointData>(point_data_attr.point_data))
          {
            auto sum_point_data =
                nostd::get<sdk::metrics::SumPointData>(point_data_attr.point_data);
            std::vector<metric_sdk::ValueType> values{sum_point_data.value_};
            SetData(values, point_data_attr.attributes, instrumentation_info.scope_, type,
                    &metric_family, data.resource_);
          }
          else
          {
            OTEL_INTERNAL_LOG_WARN(
                "[Prometheus Exporter] TranslateToPrometheus - "
                "invalid SumPointData type");
          }
        }
      }
      output.emplace_back(metric_family);
    }
  }
  return output;
}

void PrometheusExporterUtils::AddPrometheusLabel(
    std::string name,
    std::string value,
    std::vector<::prometheus::ClientMetric::Label> *labels)
{
  prometheus_client::ClientMetric::Label prometheus_label;
  prometheus_label.name  = std::move(name);
  prometheus_label.value = std::move(value);
  labels->emplace_back(std::move(prometheus_label));
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
    metric_sdk::AggregationType kind,
    bool is_monotonic)
{
  switch (kind)
  {
    case metric_sdk::AggregationType::kSum:
      if (!is_monotonic)
      {
        return prometheus_client::MetricType::Gauge;
      }
      else
      {
        return prometheus_client::MetricType::Counter;
      }
      break;
    case metric_sdk::AggregationType::kHistogram:
      return prometheus_client::MetricType::Histogram;
      break;
    case metric_sdk::AggregationType::kLastValue:
      return prometheus_client::MetricType::Gauge;
      break;
    default:
      return prometheus_client::MetricType::Untyped;
  }
}

void PrometheusExporterUtils::SetTarget(
    const sdk::metrics::ResourceMetrics &data,
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope *scope,
    std::vector<::prometheus::MetricFamily> *output)
{
  if (output == nullptr || data.resource_ == nullptr)
  {
    return;
  }

  prometheus_client::MetricFamily metric_family;
  metric_family.name = "target";
  metric_family.help = "Target metadata";
  metric_family.type = prometheus_client::MetricType::Info;
  metric_family.metric.emplace_back();

  prometheus_client::ClientMetric &metric = metric_family.metric.back();
  metric.info.value                       = 1.0;

  metric_sdk::PointAttributes empty_attributes;
  SetMetricBasic(metric, empty_attributes, scope, data.resource_);

  for (auto &label : data.resource_->GetAttributes())
  {
    AddPrometheusLabel(SanitizeName(label.first), AttributeValueToString(label.second),
                       &metric.label);
  }

  output->emplace_back(std::move(metric_family));
}

/**
 * Set metric data for:
 * sum => Prometheus Counter
 */
template <typename T>
void PrometheusExporterUtils::SetData(
    std::vector<T> values,
    const metric_sdk::PointAttributes &labels,
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope *scope,
    prometheus_client::MetricType type,
    prometheus_client::MetricFamily *metric_family,
    const opentelemetry::sdk::resource::Resource *resource)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, labels, scope, resource);
  SetValue(values, type, &metric);
}

/**
 * Set metric data for:
 * Histogram => Prometheus Histogram
 */
template <typename T>
void PrometheusExporterUtils::SetData(
    std::vector<T> values,
    const std::vector<double> &boundaries,
    const std::vector<uint64_t> &counts,
    const metric_sdk::PointAttributes &labels,
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope *scope,
    prometheus_client::MetricFamily *metric_family,
    const opentelemetry::sdk::resource::Resource *resource)
{
  metric_family->metric.emplace_back();
  prometheus_client::ClientMetric &metric = metric_family->metric.back();
  SetMetricBasic(metric, labels, scope, resource);
  SetValue(values, boundaries, counts, &metric);
}

/**
 * Set labels to metric data
 */
void PrometheusExporterUtils::SetMetricBasic(
    prometheus_client::ClientMetric &metric,
    const metric_sdk::PointAttributes &labels,
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope *scope,
    const opentelemetry::sdk::resource::Resource *resource)
{
  if (labels.empty() && nullptr == resource)
  {
    return;
  }

  // Concatenate values for keys that collide after sanitation.
  // Note that attribute keys are sorted, but sanitized keys can be out-of-order.
  // We could sort the sanitized keys again, but this seems too expensive to do
  // in this hot code path. Instead, we ignore out-of-order keys and emit a warning.
  metric.label.reserve(labels.size() + 2);
  std::string previous_key;
  for (auto const &label : labels)
  {
    auto sanitized = SanitizeLabel(label.first);
    int comparison = previous_key.compare(sanitized);
    if (metric.label.empty() || comparison < 0)  // new key
    {
      previous_key = sanitized;
      metric.label.push_back({sanitized, AttributeValueToString(label.second)});
    }
    else if (comparison == 0)  // key collision after sanitation
    {
      metric.label.back().value += ";" + AttributeValueToString(label.second);
    }
    else  // order inversion introduced by sanitation
    {
      OTEL_INTERNAL_LOG_WARN(
          "[Prometheus Exporter] SetMetricBase - "
          "the sort order of labels has changed because of sanitization: '"
          << label.first << "' became '" << sanitized << "' which is less than '" << previous_key
          << "'. Ignoring this label.");
    }
  }
  if (!scope)
  {
    return;
  }
  auto scope_name = scope->GetName();
  if (!scope_name.empty())
  {
    metric.label.emplace_back();
    metric.label.back().name  = kScopeNameKey;
    metric.label.back().value = std::move(scope_name);
  }
  auto scope_version = scope->GetVersion();
  if (!scope_version.empty())
  {
    metric.label.emplace_back();
    metric.label.back().name  = kScopeVersionKey;
    metric.label.back().value = std::move(scope_version);
  }
}

std::string PrometheusExporterUtils::AttributeValueToString(
    const opentelemetry::sdk::common::OwnedAttributeValue &value)
{
  std::string result;
  if (nostd::holds_alternative<bool>(value))
  {
    result = nostd::get<bool>(value) ? "true" : "false";
  }
  else if (nostd::holds_alternative<int>(value))
  {
    result = std::to_string(nostd::get<int>(value));
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    result = std::to_string(nostd::get<int64_t>(value));
  }
  else if (nostd::holds_alternative<unsigned int>(value))
  {
    result = std::to_string(nostd::get<unsigned int>(value));
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    result = std::to_string(nostd::get<uint64_t>(value));
  }
  else if (nostd::holds_alternative<double>(value))
  {
    result = std::to_string(nostd::get<double>(value));
  }
  else if (nostd::holds_alternative<std::string>(value))
  {
    result = nostd::get<std::string>(value);
  }
  else
  {
    OTEL_INTERNAL_LOG_WARN(
        "[Prometheus Exporter] AttributeValueToString - "
        " Nested attributes not supported - ignored");
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
  if (nostd::holds_alternative<int64_t>(value_var))
  {
    value = static_cast<double>(nostd::get<int64_t>(value_var));
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
    case prometheus_client::MetricType::Gauge: {
      metric->gauge.value = value;
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
template <typename T>
void PrometheusExporterUtils::SetValue(std::vector<T> values,
                                       const std::vector<double> &boundaries,
                                       const std::vector<uint64_t> &counts,
                                       prometheus_client::ClientMetric *metric)
{
  metric->histogram.sample_sum   = static_cast<double>(values[0]);
  metric->histogram.sample_count = static_cast<std::uint64_t>(values[1]);
  std::uint64_t cumulative       = 0;
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
