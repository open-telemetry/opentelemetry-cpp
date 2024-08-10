// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_span_builder.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

static OtlpHttpSpanBuilder singleton;

void OtlpHttpSpanBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetOtlpHttpSpanBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpSpanBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpSpanExporterConfiguration * /* model */) const
{
  // FIXME, use model
  OtlpHttpExporterOptions options;
  return OtlpHttpExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
