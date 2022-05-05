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
{}
// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<opentelemetry::sdk::trace::Recordable> OtlpHttpExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<opentelemetry::sdk::trace::Recordable>(
      new exporter::otlp::OtlpRecordable());
}

opentelemetry::sdk::common::ExportResult OtlpHttpExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept
{
  if (spans.empty())
  {
    return opentelemetry::sdk::common::ExportResult::kSuccess;
  }

  proto::collector::trace::v1::ExportTraceServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(spans, &service_request);
  return http_client_->Export(service_request);
}

#ifdef ENABLE_ASYNC_EXPORT
void OtlpHttpExporter::Export(
    const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans,
    std::function<bool(opentelemetry::sdk::common::ExportResult)> &&result_callback) noexcept
{
  if (spans.empty())
  {
    return;
  }

  proto::collector::trace::v1::ExportTraceServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(spans, &service_request);
  http_client_->Export(service_request, std::move(result_callback));
}
#endif

bool OtlpHttpExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_->Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
