// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/exporters/otlp/otlp_builder_utils.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_push_metric_builder.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/configuration/default_histogram_aggregation.h"
#include "opentelemetry/sdk/configuration/grpc_tls_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpGrpcPushMetricBuilder::Register(opentelemetry::sdk::configuration::Registry *registry)
{
  auto builder = std::make_unique<OtlpGrpcPushMetricBuilder>();
  registry->SetOtlpGrpcPushMetricExporterBuilder(std::move(builder));
}

std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> OtlpGrpcPushMetricBuilder::Build(
    const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *model) const
{
  // FIXME-SDK: default_histogram_aggregation is parsed but not implemented by the SDK.
  if (model->default_histogram_aggregation !=
      opentelemetry::sdk::configuration::DefaultHistogramAggregation::explicit_bucket_histogram)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[Otlp Grpc Exporter] default_histogram_aggregation is not supported and will be "
        "ignored");
  }

  OtlpGrpcMetricExporterOptions options(nullptr);

  const auto *tls = model->tls.get();

  options.endpoint = model->endpoint;

  options.use_ssl_credentials = OtlpBuilderUtils::GrpcUseSsl(options.endpoint, tls);

  if (tls != nullptr)
  {
    options.ssl_credentials_cacert_path = tls->ca_file;
#ifdef ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW
    options.ssl_client_key_path  = tls->key_file;
    options.ssl_client_cert_path = tls->cert_file;
#else
    if (!tls->key_file.empty() || !tls->cert_file.empty())
    {
      OTEL_INTERNAL_LOG_WARN(
          "[Otlp Grpc Exporter] mTLS client key/cert configured but the SDK was built without "
          "ENABLE_OTLP_GRPC_SSL_MTLS_PREVIEW: tls.cert_file and tls.key_file will be ignored");
    }
#endif
  }

  options.timeout = std::chrono::duration_cast<std::chrono::system_clock::duration>(
      std::chrono::seconds{model->timeout});
  options.metadata =
      OtlpBuilderUtils::ConvertHeadersConfigurationModel(model->headers.get(), model->headers_list);
  options.compression = model->compression;

  options.aggregation_temporality =
      OtlpBuilderUtils::ConvertTemporalityPreference(model->temporality_preference);

  return OtlpGrpcMetricExporterFactory::Create(options);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
