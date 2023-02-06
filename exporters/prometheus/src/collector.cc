// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/prometheus/collector.h"

namespace metric_sdk = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
/**
 * Default Constructor.
 *
 * This constructor initializes the collection for metrics to export
 * in this class with default capacity
 */
PrometheusCollector::PrometheusCollector(sdk::metrics::MetricReader *reader) : reader_(reader) {}

/**
 * Collects all metrics data from metricsToCollect collection.
 *
 * @return all metrics in the metricsToCollect snapshot
 */
std::vector<prometheus_client::MetricFamily> PrometheusCollector::Collect() const
{
  if (reader_->IsShutdown())
  {
    return {};
  }
  collection_lock_.lock();

  std::vector<prometheus_client::MetricFamily> result;
  reader_->Collect([&result](sdk::metrics::ResourceMetrics &metric_data) {
    auto prometheus_metric_data = PrometheusExporterUtils::TranslateToPrometheus(metric_data);
    for (auto &data : prometheus_metric_data)
      result.emplace_back(data);
    return true;
  });
  collection_lock_.unlock();
  return result;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
