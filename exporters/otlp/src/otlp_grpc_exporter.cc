// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
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

/**
 * Add span protobufs contained in recordables to request.
 * @param spans the spans to export
 * @param request the current request
 */
void PopulateRequest(const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans,
                     proto::collector::trace::v1::ExportTraceServiceRequest *request)
{
  auto resource_span       = request->add_resource_spans();
  auto instrumentation_lib = resource_span->add_instrumentation_library_spans();
  bool has_resource        = false;

  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<OtlpRecordable>(static_cast<OtlpRecordable *>(recordable.release()));
    *instrumentation_lib->add_spans() = std::move(rec->span());

    if (!has_resource)
    {
      *resource_span->mutable_resource() = rec->ProtoResource();
      has_resource                       = true;
    }
  }
}

static std::string get_file_contents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents;
  contents.assign((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
  finstream.close();
  return contents;
}

/**
 * Create service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<proto::collector::trace::v1::TraceService::Stub> MakeServiceStub(
    const OtlpGrpcExporterOptions &options)
{
  std::shared_ptr<grpc::Channel> channel;
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
    channel = grpc::CreateChannel(options.endpoint, grpc::SslCredentials(ssl_opts));
  }
  else
  {
    channel = grpc::CreateChannel(options.endpoint, grpc::InsecureChannelCredentials());
  }
  return proto::collector::trace::v1::TraceService::NewStub(channel);
}

// -------------------------------- Constructors --------------------------------

OtlpGrpcExporter::OtlpGrpcExporter() : OtlpGrpcExporter(OtlpGrpcExporterOptions()) {}

OtlpGrpcExporter::OtlpGrpcExporter(const OtlpGrpcExporterOptions &options)
    : options_(options), trace_service_stub_(MakeServiceStub(options))
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
  proto::collector::trace::v1::ExportTraceServiceRequest request;

  PopulateRequest(spans, &request);

  grpc::ClientContext context;
  proto::collector::trace::v1::ExportTraceServiceResponse response;

  grpc::Status status = trace_service_stub_->Export(&context, request, &response);

  if (!status.ok())
  {

    OTEL_INTERNAL_LOG_ERROR("[OTLP Exporter] Export() failed: " << status.error_message())
    return sdk::common::ExportResult::kFailure;
  }
  return sdk::common::ExportResult::kSuccess;
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
