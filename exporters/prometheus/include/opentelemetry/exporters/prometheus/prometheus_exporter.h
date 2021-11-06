// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include <string>
#  include <vector>

#  include "opentelemetry/exporters/prometheus/prometheus_collector.h"
#  include "opentelemetry/sdk/_metrics/exporter.h"
#  include "opentelemetry/sdk/_metrics/record.h"
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
namespace prometheus
{
class PrometheusExporter : public sdk::metrics::MetricsExporter
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
  sdk::common::ExportResult Export(
      const std::vector<sdk::metrics::Record> &records) noexcept override;

  /**
   * Shuts down the exporter and does cleanup.
   * Since Prometheus is a pull based interface,
   * we cannot serve data remaining in the intermediate
   * collection to to client an HTTP request being sent,
   * so we flush the data.
   */
  void Shutdown() noexcept;

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
}  // namespace prometheus
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif  // ENABLE_METRICS_PREVIEW
