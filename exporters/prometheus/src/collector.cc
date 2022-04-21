// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/prometheus/collector.h"

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
PrometheusCollector::PrometheusCollector(size_t max_collection_size)
    : max_collection_size_(max_collection_size)
{}

/**
 * Collects all metrics data from metricsToCollect collection.
 *
 * @return all metrics in the metricsToCollect snapshot
 */
std::vector<prometheus_client::MetricFamily> PrometheusCollector::Collect() const
{
  this->collection_lock_.lock();
  if (metrics_to_collect_.empty())
  {
    this->collection_lock_.unlock();
    return {};
  }

  std::vector<prometheus_client::MetricFamily> result;

  // copy the intermediate collection, and then clear it
  std::vector<std::unique_ptr<sdk::metrics::ResourceMetrics>> copied_data;
  copied_data.swap(metrics_to_collect_);
  this->collection_lock_.unlock();

  result = PrometheusExporterUtils::TranslateToPrometheus(copied_data);
  return result;
}

/**
 * This function is called by export() function and add the collection of
 * records to the metricsToCollect collection
 *
 * @param records a collection of records to add to the metricsToCollect collection
 */
void PrometheusCollector::AddMetricData(const sdk::metrics::ResourceMetrics &data)
{
  collection_lock_.lock();
  if (metrics_to_collect_.size() + 1 <= max_collection_size_)
  {
    metrics_to_collect_.emplace_back(new sdk::metrics::ResourceMetrics{data});
  }
  collection_lock_.unlock();
}

/**
 * Get the current collection in the collector.
 *
 * @return the current metrics_to_collect collection
 */
std::vector<std::unique_ptr<sdk::metrics::ResourceMetrics>> &PrometheusCollector::GetCollection()
{
  return metrics_to_collect_;
}

/**
 * Gets the maximum size of the collection.
 *
 * @return max collection size
 */
int PrometheusCollector::GetMaxCollectionSize() const
{
  return max_collection_size_;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
