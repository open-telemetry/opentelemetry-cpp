// Copyright 2023, OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/prometheus/push_exporter.h"

#include <prometheus/labels.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace metrics
{

namespace
{
static std::string SanitizePrometheusNames(std::string name, bool label)
{
  constexpr const auto replacement     = '_';
  constexpr const auto replacement_dup = '=';

  bool (*valid)(std::size_t, char);
  if (label)
  {
    valid = [](std::size_t i, char c) {
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9' && i > 0))
      {
        return true;
      }
      return false;
    };
  }
  else
  {
    valid = [](std::size_t i, char c) {
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ':' ||
          (c >= '0' && c <= '9' && i > 0))
      {
        return true;
      }
      return false;
    };
  }

  bool has_dup = false;
  for (std::size_t i = 0; i < name.size(); ++i)
  {
    if (valid(i, name[i]))
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
}  // namespace

class PrometheusPushCollector : public ::prometheus::Collectable
{
public:
  /**
   * Default Constructor.
   *
   * This constructor initializes the collection for metrics to export
   * in this class with default capacity
   */
  explicit PrometheusPushCollector(std::size_t max_collection_size = 2048)
      : max_collection_size_(max_collection_size)
  {
    metrics_to_collect_.reserve(max_collection_size_.load(std::memory_order_acquire));
  }

  /**
   * Collects all metrics data from metricsToCollect collection.
   *
   * @return all metrics in the metricsToCollect snapshot
   */
  std::vector<::prometheus::MetricFamily> Collect() const override
  {
    if (!collection_lock_.try_lock())
    {
      return {};
    }

    // copy the intermediate collection, and then clear it
    std::vector<::prometheus::MetricFamily> moved_data;
    moved_data.swap(metrics_to_collect_);
    metrics_to_collect_.reserve(max_collection_size_.load(std::memory_order_acquire));

    collection_lock_.unlock();

    return moved_data;
  }

  /**
   * This function is called by export() function and add the collection of
   * records to the metricsToCollect collection
   *
   * @param records a collection of records to add to the metricsToCollect
   * collection
   */
  void AddMetricData(const ::opentelemetry::sdk::metrics::ResourceMetrics &data)
  {
    auto translated =
        ::opentelemetry::exporter::metrics::PrometheusExporterUtils::TranslateToPrometheus(data);

    std::lock_guard<std::mutex> guard{collection_lock_};

    for (auto &item : translated)
    {
      if (metrics_to_collect_.size() + 1 <= max_collection_size_.load(std::memory_order_acquire))
      {
        // We can not use initializer lists here due to broken variadic capture
        // on GCC 4.8.5
        metrics_to_collect_.emplace_back(std::move(item));
      }
    }
  }

  /**
   * Get the current collection in the collector.
   *
   * @return the current metricsToCollect collection
   */
  std::vector<::prometheus::MetricFamily> &GetCollection() { return metrics_to_collect_; }

  /**
   * Gets the maximum size of the collection.
   *
   * @return max collection size
   */
  std::size_t GetMaxCollectionSize() const noexcept
  {
    return max_collection_size_.load(std::memory_order_acquire);
  }

private:
  /**
   * Collection of metrics data from the export() function, and to be export
   * to user when they send a pull request. This collection is a pointer
   * to a collection so Collect() is able to clear the collection, even
   * though it is a const function.
   */
  mutable std::vector<::prometheus::MetricFamily> metrics_to_collect_;

  /**
   * Maximum size of the metricsToCollect collection.
   */
  std::atomic<std::size_t> max_collection_size_;

  /*
   * Lock when operating the metricsToCollect collection
   */
  mutable std::mutex collection_lock_;
};

/**
 * Constructor - binds an exposer and collector to the exporter
 * @param address: an address for an exposer that exposes
 *  an HTTP endpoint for the exporter to connect to
 */
PrometheusPushExporter::PrometheusPushExporter(const PrometheusPushExporterOptions &options)
    : options_(options), is_shutdown_(false)
{
  ::prometheus::Labels labels;
  for (auto &label : options_.labels)
  {
    labels[SanitizePrometheusNames(label.first, true)] = label.second;
  }
  gateway_ = std::unique_ptr<::prometheus::Gateway>(
      new ::prometheus::Gateway{options_.host, options_.port, options_.jobname, labels,
                                options_.username, options_.password});
  collector_ = std::make_shared<PrometheusPushCollector>(options.max_collection_size);

  gateway_->RegisterCollectable(collector_);
}

/**
 * PrometheusPushExporter constructor with no parameters
 * Used for testing only
 */
PrometheusPushExporter::PrometheusPushExporter() : is_shutdown_(false)
{
  const_cast<PrometheusPushExporterOptions &>(options_).max_collection_size = 3;
  collector_ = std::make_shared<PrometheusPushCollector>(options_.max_collection_size);
}

::opentelemetry::sdk::metrics::AggregationTemporality
    PrometheusPushExporter::GetAggregationTemporality(
        ::opentelemetry::sdk::metrics::InstrumentType /*instrument_type*/) const noexcept
{
  // Prometheus exporter only support Cumulative
  return ::opentelemetry::sdk::metrics::AggregationTemporality::kCumulative;
}

/**
 * Exports a batch of Metric Records.
 * @param records: a collection of records to export
 * @return: returns a ReturnCode detailing a success, or type of failure
 */
::opentelemetry::sdk::common::ExportResult PrometheusPushExporter::Export(
    const ::opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept
{
  if (is_shutdown_)
  {
    return ::opentelemetry::sdk::common::ExportResult::kFailure;
  }
  else if (collector_->GetCollection().size() + data.scope_metric_data_.size() >
           collector_->GetMaxCollectionSize())
  {
    return ::opentelemetry::sdk::common::ExportResult::kFailureFull;
  }
  else if (data.scope_metric_data_.empty())
  {
    return ::opentelemetry::sdk::common::ExportResult::kFailureInvalidArgument;
  }
  else
  {
    collector_->AddMetricData(data);
    if (gateway_)
    {
      int http_code = gateway_->Push();
      if (http_code >= 200 && http_code < 300)
      {
        return ::opentelemetry::sdk::common::ExportResult::kSuccess;
      }
      return ::opentelemetry::sdk::common::ExportResult::kFailure;
    }
    return ::opentelemetry::sdk::common::ExportResult::kSuccess;
  }
}

bool PrometheusPushExporter::ForceFlush(std::chrono::microseconds /*timeout*/) noexcept
{
  return true;
}

/**
 * Shuts down the exporter and does cleanup.
 * Since Prometheus is a pull based interface,
 * we cannot serve data remaining in the intermediate
 * collection to to client an HTTP request being sent,
 * so we flush the data.
 */
bool PrometheusPushExporter::Shutdown(std::chrono::microseconds /*timeout*/) noexcept
{
  is_shutdown_ = true;

  collector_->GetCollection().clear();

  return true;
}

/**
 * Gets the maximum size of the collection.
 *
 * @return max collection size
 */
std::size_t PrometheusPushExporter::GetMaxCollectionSize() const noexcept
{
  return collector_->GetMaxCollectionSize();
}

/**
 * @return: Gets the shutdown status of the exporter
 */
bool PrometheusPushExporter::IsShutdown() const
{
  return is_shutdown_;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
