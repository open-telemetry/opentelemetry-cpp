// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_grpc_log_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#  include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"
#  include "opentelemetry/sdk_config.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
#  include "opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include <grpcpp/grpcpp.h>

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// ----------------------------- Helper functions ------------------------------

std::unique_ptr<proto::collector::logs::v1::LogsService::Stub> MakeServiceStub(
    const OtlpGrpcExporterOptions &options);

/**
 * Create log service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<proto::collector::logs::v1::LogsService::Stub> MakeLogServiceStub(
    const OtlpGrpcExporterOptions &options)
{
  return proto::collector::logs::v1::LogsService::NewStub(MakeGrpcChannel(options));
}

// -------------------------------- Constructors --------------------------------

OtlpGrpcLogExporter::OtlpGrpcLogExporter() : OtlpGrpcLogExporter(OtlpGrpcLogExporterOptions()) {}

OtlpGrpcLogExporter::OtlpGrpcLogExporter(const OtlpExporterOptions &options)
    : options_(options), log_service_stub_(MakeLogServiceStub(options))
{}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<opentelemetry::sdk::logs::Recordable> OtlpGrpcLogExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::logs::Recordable>(
      new exporter::otlp::OtlpLogRecordable());
}

opentelemetry::sdk::common::ExportResult OtlpGrpcLogExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs) noexcept
{
  proto::collector::logs::v1::ExportLogsServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(logs, &service_request);

  grpc::Status status = log_service_stub_->Export(&context, request, &response);

  if (!status.ok())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP LOG GRPC Exporter] Export() failed: " << status.error_message());
    return sdk::common::ExportResult::kFailure;
  }
  return sdk::common::ExportResult::kSuccess;
}

bool OtlpHttpLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_.Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
