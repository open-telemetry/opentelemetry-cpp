// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/prometheus/exporter.h"

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
PrometheusExporter::PrometheusExporter(const PrometheusExporterOptions &options) : options_(options)
{
  exposer_   = std::unique_ptr<::prometheus::Exposer>(new ::prometheus::Exposer{options_.url});
  collector_ = std::shared_ptr<PrometheusCollector>(new PrometheusCollector(this));

  exposer_->RegisterCollectable(collector_);
}

sdk::metrics::AggregationTemporality PrometheusExporter::GetAggregationTemporality(
    sdk::metrics::InstrumentType /* instrument_type */) const noexcept
{
  // Prometheus exporter only support Cumulative
  return sdk::metrics::AggregationTemporality::kCumulative;
}

bool PrometheusExporter::OnForceFlush(std::chrono::microseconds timeout) noexcept
{
  return true;
}

bool PrometheusExporter::OnShutDown(std::chrono::microseconds timeout) noexcept
{
  return true;
}

void PrometheusExporter::OnInitialized() noexcept {}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
