// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <string>
#  include <vector>

#  include "opentelemetry/sdk/_metrics/record.h"
#  include "prometheus/metric_family.h"

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
  static std::vector<::prometheus::MetricFamily> TranslateToPrometheus(
      const std::vector<opentelemetry::sdk::metrics::Record> &records);

private:
  /**
   * Set value to metric family according to record
   */
  static void SetMetricFamily(opentelemetry::sdk::metrics::Record &record,
                              ::prometheus::MetricFamily *metric_family);

  /**
   * Sanitize the given metric name or label according to Prometheus rule.
   *
   * This function is needed because names in OpenTelemetry can contain
   * alphanumeric characters, '_', '.', and '-', whereas in Prometheus the
   * name should only contain alphanumeric characters and '_'.
   */
  static std::string SanitizeNames(std::string name);

  /**
   * Set value to metric family for different aggregator
   */
  template <typename T>
  static void SetMetricFamilyByAggregator(
      std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> aggregator,
      std::string labels_str,
      ::prometheus::MetricFamily *metric_family);

  /**
   * Translate the OTel metric type to Prometheus metric type
   */
  static ::prometheus::MetricType TranslateType(opentelemetry::sdk::metrics::AggregatorKind kind);

  /**
   * Set metric data for:
   * Counter => Prometheus Counter
   * Gauge => Prometheus Gauge
   */
  template <typename T>
  static void SetData(std::vector<T> values,
                      const std::string &labels,
                      ::prometheus::MetricType type,
                      std::chrono::nanoseconds time,
                      ::prometheus::MetricFamily *metric_family);

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
                      ::prometheus::MetricFamily *metric_family);

  /**
   * Set metric data for:
   * MinMaxSumCount => Prometheus Gauge
   * Use Average (sum / count) as the gauge metric
   */
  static void SetData(double value,
                      const std::string &labels,
                      std::chrono::nanoseconds time,
                      ::prometheus::MetricFamily *metric_family);

  /**
   * Set metric data for:
   * Exact => Prometheus Summary
   * Sketch => Prometheus Summary
   */
  template <typename T>
  static void SetData(std::vector<T> values,
                      opentelemetry::sdk::metrics::AggregatorKind kind,
                      const std::vector<T> &quantiles,
                      const std::string &labels,
                      std::chrono::nanoseconds time,
                      ::prometheus::MetricFamily *metric_family,
                      bool do_quantile,
                      std::vector<double> quantile_points);

  /**
   * Set time and labels to metric data
   */
  static void SetMetricBasic(::prometheus::ClientMetric &metric,
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
  static std::vector<T> GetQuantilesVector(
      std::shared_ptr<opentelemetry::sdk::metrics::Aggregator<T>> aggregator,
      const std::vector<double> &quantile_points);

  /**
   * Handle Counter and Gauge.
   */
  template <typename T>
  static void SetValue(std::vector<T> values,
                       ::prometheus::MetricType type,
                       ::prometheus::ClientMetric *metric);

  /**
   * Handle Gauge from MinMaxSumCount
   */
  static void SetValue(double value, ::prometheus::ClientMetric *metric);

  /**
   * Handle Histogram
   */
  template <typename T>
  static void SetValue(std::vector<T> values,
                       std::vector<double> boundaries,
                       std::vector<int> counts,
                       ::prometheus::ClientMetric *metric);

  /**
   * Handle Exact and Sketch
   */
  template <typename T>
  static void SetValue(std::vector<T> values,
                       opentelemetry::sdk::metrics::AggregatorKind kind,
                       std::vector<T> quantiles,
                       ::prometheus::ClientMetric *metric,
                       bool do_quantile,
                       const std::vector<double> &quantile_points);
};
}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
