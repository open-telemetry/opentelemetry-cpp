// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <memory>

#include "opentelemetry/common/macros.h"
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
    : options_(options),
#ifdef ENABLE_ASYNC_EXPORT
      client_(std::make_shared<OtlpGrpcClient>()),
#endif
      log_service_stub_(OtlpGrpcClient::MakeLogsServiceStub(options))
{}

OtlpGrpcLogRecordExporter::OtlpGrpcLogRecordExporter(
    std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> stub)
    : options_(OtlpGrpcLogRecordExporterOptions()),
#ifdef ENABLE_ASYNC_EXPORT
      client_(std::make_shared<OtlpGrpcClient>()),
#endif
      log_service_stub_(std::move(stub))
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

  google::protobuf::ArenaOptions arena_options;
  // It's easy to allocate datas larger than 1024 when we populate basic resource and attributes
  arena_options.initial_block_size = 1024;
  // When in batch mode, it's easy to export a large number of spans at once, we can alloc a lager
  // block to reduce memory fragments.
  arena_options.max_block_size = 65536;
  std::unique_ptr<google::protobuf::Arena> arena{new google::protobuf::Arena{arena_options}};

  proto::collector::logs::v1::ExportLogsServiceRequest *request =
      google::protobuf::Arena::Create<proto::collector::logs::v1::ExportLogsServiceRequest>(
          arena.get());
  OtlpRecordableUtils::PopulateRequest(logs, request);

  auto context = OtlpGrpcClient::MakeClientContext(options_);
  proto::collector::logs::v1::ExportLogsServiceResponse *response =
      google::protobuf::Arena::Create<proto::collector::logs::v1::ExportLogsServiceResponse>(
          arena.get());

#ifdef ENABLE_ASYNC_EXPORT
  if (options_.max_concurrent_requests > 1)
  {
    return client_->DelegateAsyncExport(
        options_, log_service_stub_.get(), std::move(context), std::move(arena),
        std::move(*request),
        [](opentelemetry::sdk::common::ExportResult result,
           std::unique_ptr<google::protobuf::Arena> &&,
           const proto::collector::logs::v1::ExportLogsServiceRequest &request,
           proto::collector::logs::v1::ExportLogsServiceResponse *) {
          if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
          {
            OTEL_INTERNAL_LOG_ERROR("[OTLP LOG GRPC Exporter] ERROR: Export "
                                    << request.resource_logs_size()
                                    << " log(s) error: " << static_cast<int>(result));
          }
          else
          {
            OTEL_INTERNAL_LOG_DEBUG("[OTLP LOG GRPC Exporter] Export "
                                    << request.resource_logs_size() << " log(s) success");
          }
          return true;
        });
  }
  else
  {
#endif
    grpc::Status status =
        OtlpGrpcClient::DelegateExport(log_service_stub_.get(), std::move(context),
                                       std::move(arena), std::move(*request), response);

    if (!status.ok())
    {
      OTEL_INTERNAL_LOG_ERROR(
          "[OTLP LOG GRPC Exporter] Export() failed: " << status.error_message());
      return sdk::common::ExportResult::kFailure;
    }
#ifdef ENABLE_ASYNC_EXPORT
  }
#endif
  return sdk::common::ExportResult::kSuccess;
}

bool OtlpGrpcLogRecordExporter::Shutdown(
    OPENTELEMETRY_MAYBE_UNUSED std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
#ifdef ENABLE_ASYNC_EXPORT
  return client_->Shutdown(timeout);
#else
  return true;
#endif
}

bool OtlpGrpcLogRecordExporter::ForceFlush(
    OPENTELEMETRY_MAYBE_UNUSED std::chrono::microseconds timeout) noexcept
{
#ifdef ENABLE_ASYNC_EXPORT
  return client_->ForceFlush(timeout);
#else
  return true;
#endif
}

bool OtlpGrpcLogRecordExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
