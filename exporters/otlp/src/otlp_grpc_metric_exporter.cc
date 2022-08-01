// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_metric_utils.h"

#  include <mutex>
#  include "opentelemetry/ext/http/common/url_parser.h"
#  include "opentelemetry/sdk_config.h"

#  include <grpcpp/grpcpp.h>
#  include <fstream>
#  include <sstream>  // std::stringstream

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
static std::shared_ptr<grpc::Channel> MakeGrpcChannel(const OtlpGrpcMetricExporterOptions &options)
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
 * Create metrics service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<::opentelemetry::proto::collector::metrics::v1::MetricsService::Stub>
MakeMetricsServiceStub(const OtlpGrpcMetricExporterOptions &options)
{
  return proto::collector::metrics::v1::MetricsService::NewStub(MakeGrpcChannel(options));
}

// -------------------------------- Constructors --------------------------------

OtlpGrpcMetricExporter::OtlpGrpcMetricExporter()
    : OtlpGrpcMetricExporter(OtlpGrpcMetricExporterOptions())
{}

OtlpGrpcMetricExporter::OtlpGrpcMetricExporter(const OtlpGrpcMetricExporterOptions &options)
    : options_(options), metrics_service_stub_(MakeMetricsServiceStub(options))
{}

OtlpGrpcMetricExporter::OtlpGrpcMetricExporter(
    std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub)
    : options_(OtlpGrpcMetricExporterOptions()), metrics_service_stub_(std::move(stub))
{}

// ----------------------------- Exporter methods ------------------------------

opentelemetry::sdk::common::ExportResult OtlpGrpcMetricExporter::Export(
    const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept
{

  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP METRICS gRPC] Exporting "
                            << data.scope_metric_data_.size()
                            << " metric(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }
  if (data.scope_metric_data_.empty())
  {
    return sdk::common::ExportResult::kSuccess;
  }

  proto::collector::metrics::v1::ExportMetricsServiceRequest request;
  OtlpMetricUtils::PopulateRequest(data, &request);

  grpc::ClientContext context;
  proto::collector::metrics::v1::ExportMetricsServiceResponse response;

  if (options_.timeout.count() > 0)
  {
    context.set_deadline(std::chrono::system_clock::now() + options_.timeout);
  }

  for (auto &header : options_.metadata)
  {
    context.AddMetadata(header.first, header.second);
  }

  grpc::Status status = metrics_service_stub_->Export(&context, request, &response);

  if (!status.ok())
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[OTLP METRIC GRPC Exporter] Export() failed: " << status.error_message());
    return sdk::common::ExportResult::kFailure;
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif