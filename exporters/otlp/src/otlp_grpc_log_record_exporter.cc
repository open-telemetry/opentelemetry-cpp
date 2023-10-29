// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <memory>

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

// clang-format off

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
#include "opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

// clang-format on

#include "opentelemetry/sdk/common/global_log_handler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
// -------------------------------- Constructors --------------------------------

OtlpGrpcLogRecordExporter::OtlpGrpcLogRecordExporter()
    : OtlpGrpcLogRecordExporter(OtlpGrpcLogRecordExporterOptions())
{}

OtlpGrpcLogRecordExporter::OtlpGrpcLogRecordExporter(
    const OtlpGrpcLogRecordExporterOptions &options)
    : options_(options), log_service_stub_(OtlpGrpcClient::MakeLogsServiceStub(options))
{}

OtlpGrpcLogRecordExporter::OtlpGrpcLogRecordExporter(
    std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> stub)
    : options_(OtlpGrpcLogRecordExporterOptions()), log_service_stub_(std::move(stub))
{}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<opentelemetry::sdk::logs::Recordable>
OtlpGrpcLogRecordExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::logs::Recordable>(new OtlpLogRecordable());
}

opentelemetry::sdk::common::ExportResult OtlpGrpcLogRecordExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs) noexcept
{
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP gRPC log] Exporting " << logs.size()
                                                         << " log(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }
  if (logs.empty())
  {
    return sdk::common::ExportResult::kSuccess;
  }

  proto::collector::logs::v1::ExportLogsServiceRequest request;
  OtlpRecordableUtils::PopulateRequest(logs, &request);

  auto context = OtlpGrpcClient::MakeClientContext(options_);
  proto::collector::logs::v1::ExportLogsServiceResponse response;

  grpc::Status status =
      OtlpGrpcClient::DelegateExport(log_service_stub_.get(), context.get(), request, &response);

  if (!status.ok())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP LOG GRPC Exporter] Export() failed: " << status.error_message());
    return sdk::common::ExportResult::kFailure;
  }
  return sdk::common::ExportResult::kSuccess;
}

bool OtlpGrpcLogRecordExporter::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool OtlpGrpcLogRecordExporter::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  // TODO: When we implement async exporting in OTLP gRPC exporter in the future, we need wait the
  //       running exporting finished here.
  return true;
}

bool OtlpGrpcLogRecordExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
