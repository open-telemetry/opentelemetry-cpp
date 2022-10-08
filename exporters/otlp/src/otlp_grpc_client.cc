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
}  // namespace

std::shared_ptr<grpc::Channel> OtlpGrpcClient::MakeChannel(const OtlpGrpcExporterOptions &options)
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
    if (options.ssl_credentials_cacert_path.empty())
    {
      ssl_opts.pem_root_certs = options.ssl_credentials_cacert_as_string;
    }
    else
    {
      ssl_opts.pem_root_certs = GetFileContents((options.ssl_credentials_cacert_path).c_str());
    }
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
    const OtlpGrpcExporterOptions &options)
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

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
