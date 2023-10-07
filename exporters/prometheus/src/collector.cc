// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/prometheus/collector.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

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
PrometheusCollector::PrometheusCollector(sdk::metrics::MetricReader *reader,
                                         bool populate_target_info)
    : reader_(reader), populate_target_info_(populate_target_info)
{}

/**
 * Collects all metrics data from metricsToCollect collection.
 *
 * @return all metrics in the metricsToCollect snapshot
 */
std::vector<prometheus_client::MetricFamily> PrometheusCollector::Collect() const
{
  if (reader_->IsShutdown())
  {
    OTEL_INTERNAL_LOG_WARN(
        "[Prometheus Exporter] Collect: "
        "Exporter is shutdown, can not invoke collect operation.");
    return {};
  }
  collection_lock_.lock();

  std::vector<prometheus_client::MetricFamily> result;

  reader_->Collect([&result, this](sdk::metrics::ResourceMetrics &metric_data) {
    auto prometheus_metric_data =
        PrometheusExporterUtils::TranslateToPrometheus(metric_data, this->populate_target_info_);
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
