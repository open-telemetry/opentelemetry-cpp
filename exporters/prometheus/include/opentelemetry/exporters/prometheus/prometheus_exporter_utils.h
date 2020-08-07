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

#pragma once

#include <string>
#include <vector>

#include "opentelemetry/sdk/metrics/record.h"
#include "prometheus/metric_family.h"

namespace prometheus_client = ::prometheus;
namespace metric_sdk        = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace prometheus
{
/**
 * The Prometheus Utils contains utility functions for Prometheus Exporter
 */
class PrometheusExporterUtils
{
public:
  /**
   * Helper function to convert OpenTelemetry metrics data collection
   * to Prometheus metrics data collection
   *
   * @param records a collection of metrics in OpenTelemetry
   * @return a collection of translated metrics that is acceptable by Prometheus
   */
  static std::vector<prometheus_client::MetricFamily> TranslateToPrometheus(
      std::vector<metric_sdk::Record> &records);

private:
  /**
   * Set value to metric family according to record
   */
  static void SetMetricFamily(metric_sdk::Record &record,
                              prometheus_client::MetricFamily *metric_family);

  /**
   * Sanitize the given metric name or label according to Prometheus rule.
   *
   * This function is needed because names in OpenTelemetry can contain
   * alphanumeric characters, '_', '.', and '-', whereas in Prometheus the
   * name should only contain alphanumeric characters and '_'.
   */
  static std::string SanitizeNames(std::string name);

  /**
   * Determine whether the input name is a valid OTel name or not
   *
   * From the spec:
   * 1. They are non-empty strings
   * 2. They are case-insensitive
   * 3. The first character must be non-numeric, non-space, non-punctuation
   * 4. Subsequent characters must belong to the alphanumeric characters, '_', '.', and '-'.
   */
  static bool IsValidName(const std::string &name);

  /**
   * Set value to metric family for different aggregator
   */
  template <typename T>
  static void SetMetricFamilyByAggregator(nostd::shared_ptr<metric_sdk::Aggregator<T>> aggregator,
                                          std::string labels_str,
                                          prometheus_client::MetricFamily *metric_family);

  /**
   * Translate the OTel metric type to Prometheus metric type
   */
  static prometheus_client::MetricType TranslateType(metric_sdk::AggregatorKind kind);

  /**
   * Set metric data for:
   * Counter => Prometheus Counter
   * Gauge => Prometheus Gauge
   */
  template <typename T>
  static void SetData(std::vector<T> values,
                      const std::string &labels,
                      prometheus_client::MetricType type,
                      std::chrono::nanoseconds time,
                      prometheus_client::MetricFamily *metric_family);

  /**
   * Set metric data for:
   * Histogram => Prometheus Histogram
   */
  template <typename T>
  static void SetData(std::vector<T> values,
                      const std::vector<double> &boundaries,
                      const std::vector<int> &counts,
                      const std::string &labels,
                      std::chrono::nanoseconds time,
                      prometheus_client::MetricFamily *metric_family);

  /**
   * Set metric data for:
   * MinMaxSumCount => Prometheus Gauge
   * Use Average (sum / count) as the gauge metric
   */
  static void SetData(double value,
                      const std::string &labels,
                      std::chrono::nanoseconds time,
                      prometheus_client::MetricFamily *metric_family);

  /**
   * Set metric data for:
   * Exact => Prometheus Summary
   * Sketch => Prometheus Summary
   */
  template <typename T>
  static void SetData(std::vector<T> values,
                      metric_sdk::AggregatorKind kind,
                      const std::vector<T> &quantiles,
                      const std::string &labels,
                      std::chrono::nanoseconds time,
                      prometheus_client::MetricFamily *metric_family);

  /**
   * Set time and labels to metric data
   */
  static void SetMetricBasic(prometheus_client::ClientMetric &metric,
                             std::chrono::nanoseconds time,
                             const std::string &labels);

  /**
   * Parse a string of labels (key:value) into a vector of pairs
   * {,}
   * {l1:v1,l2:v2,...,}
   */
  static std::vector<std::pair<std::string, std::string>> ParseLabel(std::string labels);

  /**
   * Build a quantiles vector from aggregator
   */
  template <typename T>
  static std::vector<T> GetQuantilesVector(nostd::shared_ptr<metric_sdk::Aggregator<T>> aggregator);

  /**
   * Handle Counter and Gauge.
   */
  template <typename T>
  static void SetValue(std::vector<T> values,
                       prometheus_client::MetricType type,
                       prometheus_client::ClientMetric *metric);

  /**
   * Handle Gauge from MinMaxSumCount
   */
  static void SetValue(double value, prometheus_client::ClientMetric *metric);

  /**
   * Handle Histogram
   */
  template <typename T>
  static void SetValue(std::vector<T> values,
                       std::vector<double> boundaries,
                       std::vector<int> counts,
                       prometheus_client::ClientMetric *metric);

  /**
   * Handle Exact and Sketch
   */
  template <typename T>
  static void SetValue(std::vector<T> values,
                       metric_sdk::AggregatorKind kind,
                       std::vector<T> quantiles,
                       prometheus_client::ClientMetric *metric);
};
}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
