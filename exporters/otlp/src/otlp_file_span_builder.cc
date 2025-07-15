// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_file_span_builder.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_file_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpFileSpanBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<OtlpFileSpanBuilder>();
  registry->SetOtlpFileSpanBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpFileSpanBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpFileSpanExporterConfiguration * /* model */) const
{
  // FIXME, use model
  OtlpFileExporterOptions options;
  return OtlpFileExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
