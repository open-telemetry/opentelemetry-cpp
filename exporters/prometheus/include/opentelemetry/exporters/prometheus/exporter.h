// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include <string>
#  include <vector>

#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/exporters/prometheus/collector.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/recordable.h"
#  include "opentelemetry/version.h"
#  include "prometheus/exposer.h"

/**
 * This class is an implementation of the MetricsExporter interface and
 * exports Prometheus metrics data. Functions in this class should be
 * called by the Controller in our data pipeline.
 */

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace metrics
{
class PrometheusExporter : public sdk::metrics::MetricExporter
{
public:
  /**
   * Constructor - binds an exposer and collector to the exporter
   * @param address: an address for an exposer that exposes
   *  an HTTP endpoint for the exporter to connect to
   */
  PrometheusExporter(std::string &address);

  /**
   * Exports a batch of Metric Records.
   * @param records: a collection of records to export
   * @return: returns a ReturnCode detailing a success, or type of failure
   */
  sdk::common::ExportResult Export(const sdk::metrics::ResourceMetrics &data) noexcept override;

  /**
   * Force flush the exporter.
   */
  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  /**
   * Shuts down the exporter and does cleanup.
   * Since Prometheus is a pull based interface,
   * we cannot serve data remaining in the intermediate
   * collection to to client an HTTP request being sent,
   * so we flush the data.
   */
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

  /**
   * @return: returns a shared_ptr to
   * the PrometheusCollector instance
   */
  std::shared_ptr<PrometheusCollector> &GetCollector();

  /**
   * @return: Gets the shutdown status of the exporter
   */
  bool IsShutdown() const;

private:
  /**
   * exporter shutdown status
   */
  bool is_shutdown_;

  /**
   * Pointer to a
   * PrometheusCollector instance
   */
  std::shared_ptr<PrometheusCollector> collector_;

  /**
   * Pointer to an
   * Exposer instance
   */
  std::unique_ptr<::prometheus::Exposer> exposer_;

  /**
   * friend class for testing
   */
  friend class PrometheusExporterTest;

  /**
   * PrometheusExporter constructor with no parameters
   * Used for testing only
   */
  PrometheusExporter();
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif  // ENABLE_METRICS_PREVIEW
