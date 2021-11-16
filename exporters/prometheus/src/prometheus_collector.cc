// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <iostream>

#  include "opentelemetry/exporters/prometheus/prometheus_collector.h"

namespace metric_sdk = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace prometheus
{
/**
 * Default Constructor.
 *
 * This constructor initializes the collection for metrics to export
 * in this class with default capacity
 */
PrometheusCollector::PrometheusCollector(size_t max_collection_size)
    : max_collection_size_(max_collection_size)
{
  metrics_to_collect_ =
      std::unique_ptr<std::vector<metric_sdk::Record>>(new std::vector<metric_sdk::Record>);
}

/**
 * Collects all metrics data from metricsToCollect collection.
 *
 * @return all metrics in the metricsToCollect snapshot
 */
std::vector<prometheus_client::MetricFamily> PrometheusCollector::Collect() const
{
  this->collection_lock_.lock();
  if (metrics_to_collect_->empty())
  {
    this->collection_lock_.unlock();
    return {};
  }
  this->collection_lock_.unlock();

  std::vector<prometheus_client::MetricFamily> result;

  // copy the intermediate collection, and then clear it
  std::vector<metric_sdk::Record> copied_data;

  this->collection_lock_.lock();
  copied_data = std::vector<metric_sdk::Record>(*metrics_to_collect_);
  metrics_to_collect_->clear();
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
void PrometheusCollector::AddMetricData(const std::vector<sdk::metrics::Record> &records)
{
  if (records.empty())
  {
    return;
  }

  collection_lock_.lock();
  if (metrics_to_collect_->size() + records.size() <= max_collection_size_)
  {
    /**
     *  ValidAggregator is a lambda that checks a Record to see if its
     *  Aggregator is a valid nostd::shared_ptr and not a nullptr.
     */
    auto ValidAggregator = [](sdk::metrics::Record record) {
      auto aggregator_variant = record.GetAggregator();
      if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<int>>>(
              aggregator_variant))
      {
        auto aggregator =
            nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(aggregator_variant);
        if (!aggregator)
        {
          return false;
        }
      }
      else if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<short>>>(
                   aggregator_variant))
      {
        auto aggregator =
            nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(aggregator_variant);
        if (!aggregator)
        {
          return false;
        }
      }
      else if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<float>>>(
                   aggregator_variant))
      {
        auto aggregator =
            nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(aggregator_variant);
        if (!aggregator)
        {
          return false;
        }
      }
      else if (nostd::holds_alternative<std::shared_ptr<metric_sdk::Aggregator<double>>>(
                   aggregator_variant))
      {
        auto aggregator =
            nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(aggregator_variant);
        if (!aggregator)
        {
          return false;
        }
      }

      return true;
    };

    for (auto &r : records)
    {
      if (ValidAggregator(r))
      {
        metrics_to_collect_->emplace_back(r);
      }
      // Drop the record and write to std::cout
      else
      {
        // Cannot call non const functions on const Record r
        sdk::metrics::Record c = r;
        std::cout << "Dropped Record containing invalid aggregator with name: " + c.GetName()
                  << std::endl;
      }
    }
  }
  collection_lock_.unlock();
}

/**
 * Get the current collection in the collector.
 *
 * @return the current metrics_to_collect collection
 */
std::vector<sdk::metrics::Record> PrometheusCollector::GetCollection()
{
  return *metrics_to_collect_;
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

}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
