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

#  include "opentelemetry/ext/http/common/url_parser.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"

#  include <chrono>
#  include <fstream>
#  include <memory>

#  include <grpcpp/grpcpp.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// ----------------------------- Helper functions ------------------------------
// TODO: move exporters shared function to OTLP common library.
static std::string get_file_contents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents;
  contents.assign((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
  finstream.close();
  return contents;
}

struct OtlpGrpcExporterOptions;

/**
 * Create gRPC channel from the exporter options.
 */
static std::shared_ptr<grpc::Channel> MakeGrpcChannel(const OtlpGrpcExporterOptions &options)
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

// ----------------------------- Helper functions ------------------------------

/**
 * Create log service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<::opentelemetry::proto::collector::logs::v1::LogsService::Stub> MakeLogServiceStub(
    const OtlpGrpcExporterOptions &options)
{
  return proto::collector::logs::v1::LogsService::NewStub(MakeGrpcChannel(options));
}

// -------------------------------- Constructors --------------------------------

OtlpGrpcLogExporter::OtlpGrpcLogExporter() : OtlpGrpcLogExporter(OtlpGrpcExporterOptions()) {}

OtlpGrpcLogExporter::OtlpGrpcLogExporter(const OtlpGrpcExporterOptions &options)
    : options_(options), log_service_stub_(MakeLogServiceStub(options))
{}

OtlpGrpcLogExporter::OtlpGrpcLogExporter(
    std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface> stub)
    : options_(OtlpGrpcExporterOptions()), log_service_stub_(std::move(stub))
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

bool OtlpGrpcLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool OtlpGrpcLogExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
