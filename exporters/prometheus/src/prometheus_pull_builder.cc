// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/prometheus/exporter_factory.h"
#include "opentelemetry/exporters/prometheus/exporter_options.h"
#include "opentelemetry/exporters/prometheus/prometheus_pull_builder.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/translation_strategy.h"
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

  switch (model->translation_strategy)
  {
    case sdk::configuration::TranslationStrategy::UnderscoreEscapingWithSuffixes:
      options.without_units       = false;
      options.without_type_suffix = false;
      break;
    case sdk::configuration::TranslationStrategy::UnderscoreEscapingWithoutSuffixes:
      options.without_units       = true;
      options.without_type_suffix = true;
      break;
    case sdk::configuration::TranslationStrategy::NoUTF8EscapingWithSuffixes:
      // FIXME: no flag to disable UnderscoreEscaping
      OTEL_INTERNAL_LOG_WARN("[Prometheus Exporter] NoUTF8EscapingWithSuffixes not supported");
      options.without_units       = false;
      options.without_type_suffix = false;
      break;
    case sdk::configuration::TranslationStrategy::NoTranslation:
      // FIXME: no flag to disable UnderscoreEscaping
      OTEL_INTERNAL_LOG_WARN("[Prometheus Exporter] NoTranslation not supported");
      options.without_units       = true;
      options.without_type_suffix = true;
      break;
  }

  if (model->with_resource_constant_labels != nullptr)
  {
    // FIXME: with_resource_constant_labels
    OTEL_INTERNAL_LOG_WARN("[Prometheus Exporter] with_resource_constant_labels not supported");
  }

  return PrometheusExporterFactory::Create(options);
}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
