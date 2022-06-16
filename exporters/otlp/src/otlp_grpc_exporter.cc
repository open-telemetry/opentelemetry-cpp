// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"
#include <mutex>
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"
#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/sdk_config.h"

#include <grpcpp/grpcpp.h>
#include <fstream>
#include <sstream>  // std::stringstream

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// ----------------------------- Helper functions ------------------------------
static std::string get_file_contents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents;
  contents.assign((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
  finstream.close();
  return contents;
}

/**
 * Create gRPC channel from the exporter options.
 */
std::shared_ptr<grpc::Channel> MakeGrpcChannel(const OtlpGrpcExporterOptions &options)
{
  std::shared_ptr<grpc::Channel> channel;

  //
  // Scheme is allowed in OTLP endpoint definition, but is not allowed for creating gRPC channel.
  // Passing URI with scheme to grpc::CreateChannel could resolve the endpoint to some unexpected
  // address.
  //

  ext::http::common::UrlParser url(options.endpoint);
  if (!url.success_)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP Exporter] invalid endpoint: " << options.endpoint);

    return nullptr;
  }

  std::string grpc_target = url.host_ + ":" + std::to_string(static_cast<int>(url.port_));

  if (options.use_ssl_credentials)
  {
    grpc::SslCredentialsOptions ssl_opts;
    if (options.ssl_credentials_cacert_path.empty())
    {
      ssl_opts.pem_root_certs = options.ssl_credentials_cacert_as_string;
    }
    else
    {
      ssl_opts.pem_root_certs = get_file_contents((options.ssl_credentials_cacert_path).c_str());
    }
    channel = grpc::CreateChannel(grpc_target, grpc::SslCredentials(ssl_opts));
  }
  else
  {
    channel = grpc::CreateChannel(grpc_target, grpc::InsecureChannelCredentials());
  }

  return channel;
}

/**
 * Create service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<proto::collector::trace::v1::TraceService::Stub> MakeTraceServiceStub(
    const OtlpGrpcExporterOptions &options)
{
  return proto::collector::trace::v1::TraceService::NewStub(MakeGrpcChannel(options));
}

// -------------------------------- Constructors --------------------------------

OtlpGrpcExporter::OtlpGrpcExporter() : OtlpGrpcExporter(OtlpGrpcExporterOptions()) {}

OtlpGrpcExporter::OtlpGrpcExporter(const OtlpGrpcExporterOptions &options)
    : options_(options), trace_service_stub_(MakeTraceServiceStub(options))
{}

OtlpGrpcExporter::OtlpGrpcExporter(
    std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub)
    : options_(OtlpGrpcExporterOptions()), trace_service_stub_(std::move(stub))
{}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> OtlpGrpcExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new OtlpRecordable);
}

sdk::common::ExportResult OtlpGrpcExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP gRPC] Exporting " << spans.size()
                                                     << " span(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }
  if (spans.empty())
  {
    return sdk::common::ExportResult::kSuccess;
  }

  proto::collector::trace::v1::ExportTraceServiceRequest request;
  OtlpRecordableUtils::PopulateRequest(spans, &request);

  grpc::ClientContext context;
  proto::collector::trace::v1::ExportTraceServiceResponse response;

  if (options_.timeout.count() > 0)
  {
    context.set_deadline(std::chrono::system_clock::now() + options_.timeout);
  }

  for (auto &header : options_.metadata)
  {
    context.AddMetadata(header.first, header.second);
  }

  grpc::Status status = trace_service_stub_->Export(&context, request, &response);

  if (!status.ok())
  {

    OTEL_INTERNAL_LOG_ERROR(
        "[OTLP TRACE GRPC Exporter] Export() failed: " << status.error_message());
    return sdk::common::ExportResult::kFailure;
  }
  return sdk::common::ExportResult::kSuccess;
}

bool OtlpGrpcExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool OtlpGrpcExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
