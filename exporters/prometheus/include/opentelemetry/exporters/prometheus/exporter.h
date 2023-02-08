// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <prometheus/exposer.h>
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/exporters/prometheus/collector.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/version.h"

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

inline const std::string GetPrometheusDefaultHttpEndpoint()
{
  constexpr char kPrometheusEndpointEnv[]     = "PROMETHEUS_EXPORTER_ENDPOINT";
  constexpr char kPrometheusEndpointDefault[] = "localhost:9464";

  std::string endpoint;

  auto exists =
      opentelemetry::sdk::common::GetStringEnvironmentVariable(kPrometheusEndpointEnv, endpoint);
  return exists ? endpoint : kPrometheusEndpointDefault;
}

/**
 * Struct to hold Prometheus exporter options.
 */
struct PrometheusExporterOptions
{
  // The endpoint the Prometheus backend can collect metrics from
  std::string url = GetPrometheusDefaultHttpEndpoint();
};

class PrometheusExporter : public sdk::metrics::MetricReader
{
public:
  /**
   * Constructor - binds an exposer and collector to the exporter
   * @param options: options for an exposer that exposes
   *  an HTTP endpoint for the exporter to connect to
   */
  PrometheusExporter(const PrometheusExporterOptions &options);

  sdk::metrics::AggregationTemporality GetAggregationTemporality(
      sdk::metrics::InstrumentType instrument_type) const noexcept override;

private:
  // The configuration options associated with this exporter.
  const PrometheusExporterOptions options_;

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

  bool OnForceFlush(std::chrono::microseconds timeout) noexcept override;

  bool OnShutDown(std::chrono::microseconds timeout) noexcept override;

  void OnInitialized() noexcept override;
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
