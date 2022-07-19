// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_http_metric_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_metric_utils.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/sdk/common/global_log_handler.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpHttpMetricExporter::OtlpHttpMetricExporter()
    : OtlpHttpMetricExporter(OtlpHttpMetricExporterOptions())
{}

OtlpHttpMetricExporter::OtlpHttpMetricExporter(const OtlpHttpMetricExporterOptions &options)
    : options_(options),
      http_client_(new OtlpHttpClient(OtlpHttpClientOptions(options.url,
                                                            options.content_type,
                                                            options.json_bytes_mapping,
                                                            options.use_json_name,
                                                            options.console_debug,
                                                            options.timeout,
                                                            options.http_headers
#  ifdef ENABLE_ASYNC_EXPORT
                                                            ,
                                                            options.max_concurrent_requests,
                                                            options.max_requests_per_connection
#  endif
                                                            )))
{}

OtlpHttpMetricExporter::OtlpHttpMetricExporter(std::unique_ptr<OtlpHttpClient> http_client)
    : options_(OtlpHttpMetricExporterOptions()), http_client_(std::move(http_client))
{
  OtlpHttpMetricExporterOptions &options = const_cast<OtlpHttpMetricExporterOptions &>(options_);
  options.url                            = http_client_->GetOptions().url;
  options.content_type                   = http_client_->GetOptions().content_type;
  options.json_bytes_mapping             = http_client_->GetOptions().json_bytes_mapping;
  options.use_json_name                  = http_client_->GetOptions().use_json_name;
  options.console_debug                  = http_client_->GetOptions().console_debug;
  options.timeout                        = http_client_->GetOptions().timeout;
  options.http_headers                   = http_client_->GetOptions().http_headers;
#  ifdef ENABLE_ASYNC_EXPORT
  options.max_concurrent_requests     = http_client_->GetOptions().max_concurrent_requests;
  options.max_requests_per_connection = http_client_->GetOptions().max_requests_per_connection;
#  endif
}
// ----------------------------- Exporter methods ------------------------------

opentelemetry::sdk::common::ExportResult OtlpHttpMetricExporter::Export(
    const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept
{
  if (http_client_->IsShutdown())
  {
    std::size_t metric_count = data.instrumentation_info_metric_data_.size();
    OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                            << metric_count << " metric(s) failed, exporter is shutdown");
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  if (data.instrumentation_info_metric_data_.empty())
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  proto::collector::metrics::v1::ExportMetricsServiceRequest service_request;
  OtlpMetricUtils::PopulateRequest(data, &service_request);
  std::size_t metric_count = data.instrumentation_info_metric_data_.size();
#  ifdef ENABLE_ASYNC_EXPORT
  http_client_->Export(service_request, [metric_count](
                                            opentelemetry::sdk::common::ExportResult result) {
    if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
    {
      OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                              << metric_count << " metric(s) error: " << static_cast<int>(result));
    }
    else
    {
      OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Client] DEBUG: Export " << metric_count
                                                                  << " metric(s) success");
    }
    return true;
  });
  return opentelemetry::sdk::common::ExportResult::kSuccess;
#  else
  opentelemetry::sdk::common::ExportResult result = http_client_->Export(service_request);
  if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                            << metric_count << " metric(s) error: " << static_cast<int>(result));
  }
  else
  {
    OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Client] DEBUG: Export " << metric_count
                                                                << " metric(s) success");
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
#  endif
}

bool OtlpHttpMetricExporter::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return http_client_->ForceFlush(timeout);
}

bool OtlpHttpMetricExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_->Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
