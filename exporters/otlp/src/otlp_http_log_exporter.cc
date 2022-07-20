// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_http_log_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_log_recordable.h"
#  include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#  include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"

#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/sdk/common/global_log_handler.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpHttpLogExporter::OtlpHttpLogExporter() : OtlpHttpLogExporter(OtlpHttpLogExporterOptions()) {}

OtlpHttpLogExporter::OtlpHttpLogExporter(const OtlpHttpLogExporterOptions &options)
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

OtlpHttpLogExporter::OtlpHttpLogExporter(std::unique_ptr<OtlpHttpClient> http_client)
    : options_(OtlpHttpLogExporterOptions()), http_client_(std::move(http_client))
{
  OtlpHttpLogExporterOptions &options = const_cast<OtlpHttpLogExporterOptions &>(options_);
  options.url                         = http_client_->GetOptions().url;
  options.content_type                = http_client_->GetOptions().content_type;
  options.json_bytes_mapping          = http_client_->GetOptions().json_bytes_mapping;
  options.use_json_name               = http_client_->GetOptions().use_json_name;
  options.console_debug               = http_client_->GetOptions().console_debug;
  options.timeout                     = http_client_->GetOptions().timeout;
  options.http_headers                = http_client_->GetOptions().http_headers;
#  ifdef ENABLE_ASYNC_EXPORT
  options.max_concurrent_requests     = http_client_->GetOptions().max_concurrent_requests;
  options.max_requests_per_connection = http_client_->GetOptions().max_requests_per_connection;
#  endif
}
// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<opentelemetry::sdk::logs::Recordable> OtlpHttpLogExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::logs::Recordable>(
      new exporter::otlp::OtlpLogRecordable());
}

opentelemetry::sdk::common::ExportResult OtlpHttpLogExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs) noexcept
{
  if (http_client_->IsShutdown())
  {
    std::size_t log_count = logs.size();
    OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                            << log_count << " log(s) failed, exporter is shutdown");
    return opentelemetry::sdk::common::ExportResult::kFailure;
  }

  if (logs.empty())
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  proto::collector::logs::v1::ExportLogsServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(logs, &service_request);
  std::size_t log_count = logs.size();
#  ifdef ENABLE_ASYNC_EXPORT
  http_client_->Export(
      service_request, [log_count](opentelemetry::sdk::common::ExportResult result) {
        if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
        {
          OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                                  << log_count << " log(s) error: " << static_cast<int>(result));
        }
        else
        {
          OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Client] DEBUG: Export " << log_count
                                                                      << " log(s) success");
        }
        return true;
      });
  return opentelemetry::sdk::common::ExportResult::kSuccess;
#  else
  opentelemetry::sdk::common::ExportResult result = http_client_->Export(service_request);
  if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP HTTP Client] ERROR: Export "
                            << log_count << " log(s) error: " << static_cast<int>(result));
  }
  else
  {
    OTEL_INTERNAL_LOG_DEBUG("[OTLP HTTP Client] DEBUG: Export " << log_count << " log(s) success");
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
#  endif
}

bool OtlpHttpLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_->Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
