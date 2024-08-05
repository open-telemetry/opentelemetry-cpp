// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <string>

#include "opentelemetry/exporters/prometheus/exporter_factory.h"
#include "opentelemetry/exporters/prometheus/exporter_options.h"
#include "opentelemetry/exporters/prometheus/prometheus_pull_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

static PrometheusPullBuilder singleton;

void PrometheusPullBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetPrometheusPullMetricExporterBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> PrometheusPullBuilder::Build(
    const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model) const
{
  opentelemetry::exporter::metrics::PrometheusExporterOptions options;

#ifdef LATER
  // Expected
  options.url                  = model->xxx;
  options.populate_target_info = model->xxx;
  options.without_otel_scope   = model->xxx;

  // Configuration model
  options.xxx = model->host;
  options.xxx = model->port;
  options.xxx = model->without_units;
  options.xxx = model->without_type_suffix;
  options.xxx = model->without_scope_info;
#endif

  return PrometheusExporterFactory::Create(options);
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
