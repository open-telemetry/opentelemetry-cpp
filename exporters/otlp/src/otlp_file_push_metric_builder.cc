// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_file_push_metric_builder.h"
#include "opentelemetry/exporters/otlp/otlp_file_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_metric_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpFilePushMetricBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<OtlpFilePushMetricBuilder>();
  registry->SetOtlpFilePushMetricExporterBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> OtlpFilePushMetricBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpFilePushMetricExporterConfiguration * /* model */)
    const
{
  // FIXME, use model
  OtlpFileMetricExporterOptions options;
  return OtlpFileMetricExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
