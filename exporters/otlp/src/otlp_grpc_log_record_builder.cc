// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_builder.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpGrpcLogRecordBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<OtlpGrpcLogRecordBuilder>();
  registry->SetOtlpGrpcLogRecordBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> OtlpGrpcLogRecordBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpGrpcLogRecordExporterConfiguration * /* model */)
    const
{
  // FIXME, use model
  OtlpGrpcLogRecordExporterOptions options;
  return OtlpGrpcLogRecordExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
