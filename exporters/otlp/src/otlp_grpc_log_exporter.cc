// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_grpc_log_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#  include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

// clang-format off

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
#  include "opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

// clang-format on

#  include "opentelemetry/sdk/common/global_log_handler.h"

#  include <chrono>
#  include <memory>

#  include <grpcpp/grpcpp.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

struct OtlpGrpcExporterOptions;
std::shared_ptr<grpc::Channel> MakeGrpcChannel(const OtlpGrpcExporterOptions &options);

// ----------------------------- Helper functions ------------------------------

/**
 * Create log service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<::opentelemetry::proto::collector::logs::v1::LogsService::Stub> MakeLogServiceStub(
    const OtlpGrpcExporterOptions &options)
{
  std::shared_ptr<grpc::Channel> channel = MakeGrpcChannel(options);
  return proto::collector::logs::v1::LogsService::NewStub(channel);
}

// -------------------------------- Constructors --------------------------------

OtlpGrpcLogExporter::OtlpGrpcLogExporter() : OtlpGrpcLogExporter(OtlpGrpcExporterOptions()) {}

OtlpGrpcLogExporter::OtlpGrpcLogExporter(const OtlpGrpcExporterOptions &options)
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
  proto::collector::logs::v1::ExportLogsServiceRequest request;
  OtlpRecordableUtils::PopulateRequest(logs, &request);

  grpc::ClientContext context;
  proto::collector::logs::v1::ExportLogsServiceResponse response;

  if (options_.timeout.count() > 0)
  {
    context.set_deadline(std::chrono::system_clock::now() + options_.timeout);
  }

  for (auto &header : options_.metadata)
  {
    context.AddMetadata(header.first, header.second);
  }
  grpc::Status status = log_service_stub_->Export(&context, request, &response);

  if (!status.ok())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP LOG GRPC Exporter] Export() failed: " << status.error_message());
    return sdk::common::ExportResult::kFailure;
  }
  return sdk::common::ExportResult::kSuccess;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
