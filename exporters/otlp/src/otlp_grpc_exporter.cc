// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_utils.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "google/protobuf/arena.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
// -------------------------------- Constructors --------------------------------

OtlpGrpcExporter::OtlpGrpcExporter() : OtlpGrpcExporter(OtlpGrpcExporterOptions()) {}

OtlpGrpcExporter::OtlpGrpcExporter(const OtlpGrpcExporterOptions &options) : options_(options)
{
  client_                 = OtlpGrpcClientFactory::Create(options_);
  client_reference_guard_ = OtlpGrpcClientFactory::CreateReferenceGuard();
  client_->AddReference(*client_reference_guard_, options_);

  trace_service_stub_ = client_->MakeTraceServiceStub();
}

OtlpGrpcExporter::OtlpGrpcExporter(
    std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub)
    : options_(OtlpGrpcExporterOptions()), trace_service_stub_(std::move(stub))
{
  client_                 = OtlpGrpcClientFactory::Create(options_);
  client_reference_guard_ = OtlpGrpcClientFactory::CreateReferenceGuard();
  client_->AddReference(*client_reference_guard_, options_);
}

OtlpGrpcExporter::OtlpGrpcExporter(const OtlpGrpcExporterOptions &options,
                                   const std::shared_ptr<OtlpGrpcClient> &client)
    : options_(options),
      client_(client),
      client_reference_guard_(OtlpGrpcClientFactory::CreateReferenceGuard())
{
  client_->AddReference(*client_reference_guard_, options_);

  trace_service_stub_ = client_->MakeTraceServiceStub();
}

OtlpGrpcExporter::OtlpGrpcExporter(
    std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub,
    const std::shared_ptr<OtlpGrpcClient> &client)
    : options_(OtlpGrpcExporterOptions()),
      client_(client),
      client_reference_guard_(OtlpGrpcClientFactory::CreateReferenceGuard()),
      trace_service_stub_(std::move(stub))
{
  client_->AddReference(*client_reference_guard_, options_);
}

OtlpGrpcExporter::~OtlpGrpcExporter()
{
  if (client_)
  {
    client_->RemoveReference(*client_reference_guard_);
  }
}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> OtlpGrpcExporter::MakeRecordable() noexcept
{
  return std::make_unique<OtlpRecordable>(
      recordable_arena_.Get(), options_.max_attributes, options_.max_events, options_.max_links,
      options_.max_attributes_per_event, options_.max_attributes_per_link);
}

sdk::common::ExportResult OtlpGrpcExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  std::shared_ptr<OtlpGrpcClient> client = client_;
  if (isShutdown() || !client)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP gRPC] Exporting " << spans.size()
                                                     << " span(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  if (!trace_service_stub_)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP gRPC] Exporting "
                            << spans.size() << " span(s) failed, service stub unavailable");
    return sdk::common::ExportResult::kFailure;
  }

  if (spans.empty())
  {
    return sdk::common::ExportResult::kSuccess;
  }

  // The request goes on the Arena the recordables since the last export were created on, so
  // PopulateRequest moves their messages instead of copying them. The asynchronous result callback
  // keeps a reference to it until the call completes.
  std::shared_ptr<google::protobuf::Arena> request_arena = recordable_arena_.Rotate();
  // The response has an Arena of its own, whose ownership transfers to the gRPC client until the
  // call completes.
  std::unique_ptr<google::protobuf::Arena> arena = std::make_unique<google::protobuf::Arena>();

  proto::collector::trace::v1::ExportTraceServiceRequest *request =
      google::protobuf::Arena::Create<proto::collector::trace::v1::ExportTraceServiceRequest>(
          request_arena.get());
  OtlpRecordableUtils::PopulateRequest(spans, request);

  auto context = OtlpGrpcClient::MakeClientContext(options_);

#ifdef ENABLE_ASYNC_EXPORT
  if (options_.max_concurrent_requests > 1)
  {
    return client->DelegateAsyncExport(
        options_, trace_service_stub_.get(), std::move(context), std::move(arena), request,
        // Capture the trace_service_stub_ to ensure it is not destroyed before the callback is
        // called, and request_arena so the request stays alive until the call completes.
        [trace_service_stub = trace_service_stub_, request_arena](
            opentelemetry::sdk::common::ExportResult result,
            std::unique_ptr<google::protobuf::Arena> &&arena,
            const proto::collector::trace::v1::ExportTraceServiceRequest &request,
            proto::collector::trace::v1::ExportTraceServiceResponse *response) {
          auto trace_arena = std::move(arena);
          if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
          {
            OTEL_INTERNAL_LOG_ERROR("[OTLP TRACE GRPC Exporter] ERROR: Export "
                                    << request.resource_spans_size()
                                    << " trace span(s) error: " << static_cast<int>(result));
          }
          else if (response->has_partial_success() &&
                   (response->partial_success().rejected_spans() != 0 ||
                    !response->partial_success().error_message().empty()))
          {
            const auto &partial = response->partial_success();
            OTEL_INTERNAL_LOG_ERROR("[OTLP TRACE GRPC Exporter] Export partial success: "
                                    << partial.rejected_spans() << " span(s) rejected: \""
                                    << partial.error_message() << "\"");
          }
          else
          {
            OTEL_INTERNAL_LOG_DEBUG("[OTLP TRACE GRPC Exporter] Export "
                                    << request.resource_spans_size() << " trace span(s) success");
          }
          return true;
        });
  }
  else
  {
#endif
    const auto resource_spans_size = request->resource_spans_size();
    proto::collector::trace::v1::ExportTraceServiceResponse *response =
        google::protobuf::Arena::Create<proto::collector::trace::v1::ExportTraceServiceResponse>(
            arena.get());
    grpc::Status status = OtlpGrpcClient::DelegateExport(
        trace_service_stub_.get(), std::move(context), std::move(arena), request, response,
        [resource_spans_size](std::unique_ptr<google::protobuf::Arena> &&arena,
                              proto::collector::trace::v1::ExportTraceServiceResponse *response) {
          auto trace_arena = std::move(arena);
          if (response->has_partial_success() &&
              (response->partial_success().rejected_spans() != 0 ||
               !response->partial_success().error_message().empty()))
          {
            const auto &partial = response->partial_success();
            OTEL_INTERNAL_LOG_ERROR("[OTLP TRACE GRPC Exporter] Export partial success: "
                                    << partial.rejected_spans() << " span(s) rejected: \""
                                    << partial.error_message() << "\"");
          }
          else
          {
            OTEL_INTERNAL_LOG_DEBUG("[OTLP TRACE GRPC Exporter] Export "
                                    << resource_spans_size << " trace span(s) success");
          }
        });
    if (!status.ok())
    {
      OTEL_INTERNAL_LOG_ERROR("[OTLP TRACE GRPC Exporter] Export() failed with status_code: \""
                              << grpc_utils::grpc_status_code_to_string(status.error_code())
                              << "\" error_message: \"" << status.error_message() << "\"");
      return sdk::common::ExportResult::kFailure;
    }
#ifdef ENABLE_ASYNC_EXPORT
  }
#endif
  return sdk::common::ExportResult::kSuccess;
}

bool OtlpGrpcExporter::ForceFlush(
    OPENTELEMETRY_MAYBE_UNUSED std::chrono::microseconds timeout) noexcept
{
  // Maybe already shutdown, we need to keep thread-safety here.
  std::shared_ptr<OtlpGrpcClient> client = client_;
  if (!client)
  {
    return true;
  }
  return client->ForceFlush(timeout);
}

bool OtlpGrpcExporter::Shutdown(
    OPENTELEMETRY_MAYBE_UNUSED std::chrono::microseconds timeout) noexcept
{
  is_shutdown_ = true;
  recordable_arena_.Release();
  // Maybe already shutdown, we need to keep thread-safety here.
  std::shared_ptr<OtlpGrpcClient> client;
  client.swap(client_);
  if (!client)
  {
    return true;
  }
  return client->Shutdown(*client_reference_guard_, timeout);
}

bool OtlpGrpcExporter::isShutdown() const noexcept
{
  return is_shutdown_;
}

const std::shared_ptr<OtlpGrpcClient> &OtlpGrpcExporter::GetClient() const noexcept
{
  return client_;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
