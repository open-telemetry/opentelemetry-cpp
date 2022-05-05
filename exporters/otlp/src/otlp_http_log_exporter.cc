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
{}
// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<opentelemetry::sdk::logs::Recordable> OtlpHttpLogExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::logs::Recordable>(
      new exporter::otlp::OtlpLogRecordable());
}

opentelemetry::sdk::common::ExportResult OtlpHttpLogExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs) noexcept
{
  if (logs.empty())
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }
  proto::collector::logs::v1::ExportLogsServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(logs, &service_request);
  return http_client_->Export(service_request);
}

#  ifdef ENABLE_ASYNC_EXPORT
void OtlpHttpLogExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs,
    std::function<bool(opentelemetry::sdk::common::ExportResult)> &&result_callback) noexcept
{
  if (logs.empty())
  {
    return;
  }
  proto::collector::logs::v1::ExportLogsServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(logs, &service_request);
  http_client_->Export(service_request, std::move(result_callback));
}
#  endif

bool OtlpHttpLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_->Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
