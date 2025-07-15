// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_push_metric_builder.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_metric_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

static OtlpHttpPushMetricBuilder singleton;

void OtlpHttpPushMetricBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  registry->SetOtlpHttpPushMetricExporterBuilder(&singleton);
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
