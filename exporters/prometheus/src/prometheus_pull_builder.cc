// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/prometheus/exporter_factory.h"
#include "opentelemetry/exporters/prometheus/exporter_options.h"
#include "opentelemetry/exporters/prometheus/prometheus_pull_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

void PrometheusPullBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<PrometheusPullBuilder>();
  registry->SetPrometheusPullMetricExporterBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> PrometheusPullBuilder::Build(
    const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model) const
{
  opentelemetry::exporter::metrics::PrometheusExporterOptions options(nullptr);

  std::string url(model->host);
  url.append(":");
  url.append(std::to_string(model->port));

  options.url                  = url;
  options.populate_target_info = true;
  options.without_otel_scope   = model->without_scope_info;
  options.without_units        = model->without_units;
  options.without_type_suffix  = model->without_type_suffix;

  return PrometheusExporterFactory::Create(options);
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
