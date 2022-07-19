// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/sdk/common/global_log_handler.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpHttpExporter::OtlpHttpExporter() : OtlpHttpExporter(OtlpHttpExporterOptions()) {}

OtlpHttpExporter::OtlpHttpExporter(const OtlpHttpExporterOptions &options)
    : options_(options),
      http_client_(new OtlpHttpClient(OtlpHttpClientOptions(options.url,
                                                            options.content_type,
                                                            options.json_bytes_mapping,
                                                            options.use_json_name,
                                                            options.console_debug,
                                                            options.timeout,
                                                            options.http_headers
#ifdef ENABLE_ASYNC_EXPORT
                                                            ,
                                                            options.max_concurrent_requests,
                                                            options.max_requests_per_connection
#endif
                                                            )))
{}

OtlpHttpExporter::OtlpHttpExporter(std::unique_ptr<OtlpHttpClient> http_client)
    : options_(OtlpHttpExporterOptions()), http_client_(std::move(http_client))
{
  OtlpHttpExporterOptions &options = const_cast<OtlpHttpExporterOptions &>(options_);
  options.url                      = http_client_->GetOptions().url;
  options.content_type             = http_client_->GetOptions().content_type;
  options.json_bytes_mapping       = http_client_->GetOptions().json_bytes_mapping;
  options.use_json_name            = http_client_->GetOptions().use_json_name;
  options.console_debug            = http_client_->GetOptions().console_debug;
  options.timeout                  = http_client_->GetOptions().timeout;
  options.http_headers             = http_client_->GetOptions().http_headers;
#ifdef ENABLE_ASYNC_EXPORT
  options.max_concurrent_requests     = http_client_->GetOptions().max_concurrent_requests;
  options.max_requests_per_connection = http_client_->GetOptions().max_requests_per_connection;
#endif
}
// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<opentelemetry::sdk::trace::Recordable> OtlpHttpExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::trace::Recordable>(
      new exporter::otlp::OtlpRecordable());
}

opentelemetry::sdk::common::ExportResult OtlpHttpExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept
{
  if (http_client_->IsShutdown())
  {
    std::size_t span_count = spans.size();
    OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                            << span_count << " trace span(s) failed, exporter is shutdown");
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  if (spans.empty())
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  proto::collector::trace::v1::ExportTraceServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(spans, &service_request);
  std::size_t span_count = spans.size();
#ifdef ENABLE_ASYNC_EXPORT
  http_client_->Export(
      service_request, [span_count](opentelemetry::sdk::common::ExportResult result) {
        if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
        {
          OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                                  << span_count
                                  << " trace span(s) error: " << static_cast<int>(result));
        }
        else
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Client] DEBUG: Export " << span_count
                                                                      << " trace span(s) success");
        }
        return true;
      });
  return opentelemetry::sdk::common::ExportResult::kSuccess;
#else
  opentelemetry::sdk::common::ExportResult result = http_client_->Export(service_request);
  if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                            << span_count << " trace span(s) error: " << static_cast<int>(result));
  }
  else
  {
    OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Client] DEBUG: Export " << span_count
                                                                << " trace span(s) success");
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
#endif
}

bool OtlpHttpExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_->Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
