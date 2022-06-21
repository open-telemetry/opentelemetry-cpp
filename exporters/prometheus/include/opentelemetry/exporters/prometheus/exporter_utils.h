// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifndef ENABLE_METRICS_PREVIEW

#  include <prometheus/metric_family.h>
#  include <string>
#  include <vector>
#  include "opentelemetry/metrics/provider.h"
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/version.h"

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
   * Sanitize the given metric name or label according to Prometheus rule.
   *
   * This function is needed because names in OpenTelemetry can contain
   * alphanumeric characters, '_', '.', and '-', whereas in Prometheus the
   * name should only contain alphanumeric characters and '_'.
   */
  static std::string SanitizeNames(std::string name);

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
                      const opentelemetry::sdk::metrics::PointAttributes &labels,
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
                      const opentelemetry::sdk::metrics::PointAttributes &labels,
                      std::chrono::nanoseconds time,
                      ::prometheus::MetricFamily *metric_family);

  /**
   * Set time and labels to metric data
   */
  static void SetMetricBasic(::prometheus::ClientMetric &metric,
                             std::chrono::nanoseconds time,
                             const opentelemetry::sdk::metrics::PointAttributes &labels);

  /**
   * Convert attribute value to string
   */
  static std::string AttributeValueToString(
      const opentelemetry::sdk::common::OwnedAttributeValue &value);

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
