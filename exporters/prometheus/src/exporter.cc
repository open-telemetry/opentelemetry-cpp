// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/exporters/prometheus/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace metrics
{
/**
 * Constructor - binds an exposer and collector to the exporter
 * @param address: an address for an exposer that exposes
 *  an HTTP endpoint for the exporter to connect to
 */
PrometheusExporter::PrometheusExporter(const PrometheusExporterOptions &options)
    : options_(options), is_shutdown_(false)
{
  exposer_   = std::unique_ptr<::prometheus::Exposer>(new ::prometheus::Exposer{options_.url});
  collector_ = std::shared_ptr<PrometheusCollector>(new PrometheusCollector);

  exposer_->RegisterCollectable(collector_);
}

/**
 * PrometheusExporter constructor with no parameters
 * Used for testing only
 */
PrometheusExporter::PrometheusExporter() : is_shutdown_(false)
{
  collector_ = std::unique_ptr<PrometheusCollector>(new PrometheusCollector);
}

/**
 * Exports a batch of Metric Records.
 * @param records: a collection of records to export
 * @return: returns a ReturnCode detailing a success, or type of failure
 */
sdk::common::ExportResult PrometheusExporter::Export(
    const sdk::metrics::ResourceMetrics &data) noexcept
{
  if (is_shutdown_)
  {
    return sdk::common::ExportResult::kFailure;
  }
  else if (collector_->GetCollection().size() + 1 > (size_t)collector_->GetMaxCollectionSize())
  {
    return sdk::common::ExportResult::kFailureFull;
  }
  else
  {
    collector_->AddMetricData(data);
    return sdk::common::ExportResult::kSuccess;
  }
  return sdk::common::ExportResult::kSuccess;
}

bool PrometheusExporter::ForceFlush(std::chrono::microseconds timeout) noexcept
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
bool PrometheusExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  is_shutdown_ = true;
  return true;

  collector_->GetCollection().clear();
}

/**
 * @return: returns a shared_ptr to
 * the PrometheusCollector instance
 */
std::shared_ptr<PrometheusCollector> &PrometheusExporter::GetCollector()
{
  return collector_;
}

/**
 * @return: Gets the shutdown status of the exporter
 */
bool PrometheusExporter::IsShutdown() const
{
  return is_shutdown_;
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif  // ENABLE_METRICS_PREVIEW
