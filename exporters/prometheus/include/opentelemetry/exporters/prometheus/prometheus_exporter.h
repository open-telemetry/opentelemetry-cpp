/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "opentelemetry/sdk/metrics/exporter.h"
#include "opentelemetry/sdk/metrics/record.h"
#include "opentelemetry/version.h"
#include "prometheus/exposer.h"
#include "prometheus_collector.h"

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
  sdk::metrics::ExportResult Export(
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
