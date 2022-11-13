// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

// MUST be first (absl)
#  include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter.h"

#  include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpGrpcLogRecordExporterFactory::Create()
{
  OtlpGrpcExporterOptions options;
  return Create(options);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpGrpcLogRecordExporterFactory::Create(const OtlpGrpcExporterOptions &options)
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter(
      new OtlpGrpcLogRecordExporter(options));
  return exporter;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
