// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Make sure to include GRPC exporter first because otherwise Abseil may create
// ambiguity with `nostd::variant`. See issue:
// https://github.com/open-telemetry/opentelemetry-cpp/issues/880
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpGrpcExporterFactory::Build()
{
  OtlpGrpcExporterOptions options;
  return Build(options);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpGrpcExporterFactory::Build(
    const OtlpGrpcExporterOptions &options)
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter(new OtlpGrpcExporter(options));
  return std::move(exporter);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
