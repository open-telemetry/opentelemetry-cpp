// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#ifndef ENABLE_METRICS_PREVIEW

#  include <string>
#  include <vector>

#  include "opentelemetry/metrics/provider.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/sdk/metrics/meter_provider.h"
#  include "opentelemetry/version.h"
#  include "prometheus/metric_family.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
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
      const std::vector<std::unique_ptr<sdk::metrics::ResourceMetrics>> &data);

private:
  /**
   * Set value to metric family according to record
   */
  static void SetMetricFamily(sdk::metrics::ResourceMetrics &data,
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
  static void SetMetricFamilyByAggregator(const sdk::metrics::ResourceMetrics &data,
                                          std::string labels_str,
                                          ::prometheus::MetricFamily *metric_family);

  static opentelemetry::sdk::metrics::AggregationType getAggregationType(
      const opentelemetry::sdk::metrics::PointType &point_type);

  /**
   * Translate the OTel metric type to Prometheus metric type
   */
  static ::prometheus::MetricType TranslateType(opentelemetry::sdk::metrics::AggregationType kind);

  /**
   * Set metric data for:
   * Counter => Prometheus Counter
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
                      const opentelemetry::sdk::metrics::ListType &boundaries,
                      const std::vector<uint64_t> &counts,
                      const std::string &labels,
                      std::chrono::nanoseconds time,
                      ::prometheus::MetricFamily *metric_family);

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
  template <typename T, typename U>
  static void SetValue(std::vector<T> values,
                       const std::list<U> &boundaries,
                       const std::vector<uint64_t> &counts,
                       ::prometheus::ClientMetric *metric);
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
