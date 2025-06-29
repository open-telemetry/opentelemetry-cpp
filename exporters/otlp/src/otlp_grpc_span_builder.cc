// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_span_builder.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

static OtlpGrpcSpanBuilder singleton;

void OtlpGrpcSpanBuilder::Register(opentelemetry::sdk::init::Registry *registry)
{
  registry->SetOtlpGrpcSpanBuilder(&singleton);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpGrpcSpanBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpGrpcSpanExporterConfiguration * /* model */) const
{
  // FIXME, use model
  OtlpGrpcExporterOptions options;
  return OtlpGrpcExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
