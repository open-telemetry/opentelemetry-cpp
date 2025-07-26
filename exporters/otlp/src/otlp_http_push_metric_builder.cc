// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <utility>

#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_push_metric_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpHttpPushMetricBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<OtlpHttpPushMetricBuilder>();
  registry->SetOtlpHttpPushMetricExporterBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> OtlpHttpPushMetricBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpHttpPushMetricExporterConfiguration * /* model */)
    const
{
  // FIXME, use model
  OtlpHttpMetricExporterOptions options;
  return OtlpHttpMetricExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
