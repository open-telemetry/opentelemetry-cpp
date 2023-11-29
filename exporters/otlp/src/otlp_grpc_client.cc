// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"

#if defined(HAVE_GSL)
#  include <gsl/gsl>
#else
#  include <assert.h>
#endif

#include <fstream>
#include <iterator>
#include <memory>
#include <string>

#include "opentelemetry/ext/http/common/url_parser.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{
// ----------------------------- Helper functions ------------------------------
static std::string GetFileContents(const char *fpath)
{
  std::ifstream finstream(fpath);
  std::string contents;
  contents.assign((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
  finstream.close();
  return contents;
}

// If the file path is non-empty, returns the contents of the file. Otherwise returns contents.
static std::string GetFileContentsOrInMemoryContents(const std::string &file_path,
                                                     const std::string &contents)
{
  if (!file_path.empty())
  {
    return GetFileContents(file_path.c_str());
  }
  return contents;
}

}  // namespace

std::shared_ptr<grpc::Channel> OtlpGrpcClient::MakeChannel(const OtlpGrpcClientOptions &options)
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
    OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC Client] invalid endpoint: " << options.endpoint);

    return nullptr;
  }

  std::string grpc_target = url.host_ + ":" + std::to_string(static_cast<int>(url.port_));
  grpc::ChannelArguments grpc_arguments;
  grpc_arguments.SetUserAgentPrefix(options.user_agent);

  if (options.use_ssl_credentials)
  {
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = GetFileContentsOrInMemoryContents(
        options.ssl_credentials_cacert_path, options.ssl_credentials_cacert_as_string);
#if ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW
    ssl_opts.pem_private_key = GetFileContentsOrInMemoryContents(options.ssl_client_key_path,
                                                                 options.ssl_client_key_string);
    ssl_opts.pem_cert_chain  = GetFileContentsOrInMemoryContents(options.ssl_client_cert_path,
                                                                options.ssl_client_cert_string);
#endif
    channel =
        grpc::CreateCustomChannel(grpc_target, grpc::SslCredentials(ssl_opts), grpc_arguments);
  }
  else
  {
    channel =
        grpc::CreateCustomChannel(grpc_target, grpc::InsecureChannelCredentials(), grpc_arguments);
  }

  return channel;
}

std::unique_ptr<grpc::ClientContext> OtlpGrpcClient::MakeClientContext(
    const OtlpGrpcClientOptions &options)
{
  std::unique_ptr<grpc::ClientContext> context{new grpc::ClientContext()};
  if (!context)
  {
    return context;
  }

  if (options.timeout.count() > 0)
  {
    context->set_deadline(std::chrono::system_clock::now() + options.timeout);
  }

  for (auto &header : options.metadata)
  {
    context->AddMetadata(header.first, header.second);
  }

  return context;
}

std::unique_ptr<grpc::CompletionQueue> OtlpGrpcClient::MakeCompletionQueue()
{
  return std::unique_ptr<grpc::CompletionQueue>(new grpc::CompletionQueue());
}

std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface>
OtlpGrpcClient::MakeTraceServiceStub(const OtlpGrpcClientOptions &options)
{
  return proto::collector::trace::v1::TraceService::NewStub(MakeChannel(options));
}

std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface>
OtlpGrpcClient::MakeMetricsServiceStub(const OtlpGrpcClientOptions &options)
{
  return proto::collector::metrics::v1::MetricsService::NewStub(MakeChannel(options));
}

std::unique_ptr<proto::collector::logs::v1::LogsService::StubInterface>
OtlpGrpcClient::MakeLogsServiceStub(const OtlpGrpcClientOptions &options)
{
  return proto::collector::logs::v1::LogsService::NewStub(MakeChannel(options));
}

grpc::Status OtlpGrpcClient::DelegateExport(
    proto::collector::trace::v1::TraceService::StubInterface *stub,
    grpc::ClientContext *context,
    const proto::collector::trace::v1::ExportTraceServiceRequest &request,
    proto::collector::trace::v1::ExportTraceServiceResponse *response)
{
  return stub->Export(context, request, response);
}

grpc::Status OtlpGrpcClient::DelegateExport(
    proto::collector::metrics::v1::MetricsService::StubInterface *stub,
    grpc::ClientContext *context,
    const proto::collector::metrics::v1::ExportMetricsServiceRequest &request,
    proto::collector::metrics::v1::ExportMetricsServiceResponse *response)
{
  return stub->Export(context, request, response);
}

grpc::Status OtlpGrpcClient::DelegateExport(
    proto::collector::logs::v1::LogsService::StubInterface *stub,
    grpc::ClientContext *context,
    const proto::collector::logs::v1::ExportLogsServiceRequest &request,
    proto::collector::logs::v1::ExportLogsServiceResponse *response)
{
  return stub->Export(context, request, response);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
