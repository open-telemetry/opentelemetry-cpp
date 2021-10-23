// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_recordable.h"
#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

OtlpHttpExporter::OtlpHttpExporter() : OtlpHttpExporter(OtlpHttpExporterOptions()) {}

OtlpHttpExporter::OtlpHttpExporter(const OtlpHttpExporterOptions &options)
    : options_(options),
      http_client_(OtlpHttpClientOptions(options.url,
                                         options.content_type,
                                         options.json_bytes_mapping,
                                         options.use_json_name,
                                         options.console_debug,
                                         options.timeout,
                                         options.http_headers))
{}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> OtlpHttpExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new exporter::otlp::OtlpRecordable());
}

sdk::common::ExportResult OtlpHttpExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  proto::collector::trace::v1::ExportTraceServiceRequest service_request;
  OtlpRecordableUtils::PopulateRequest(spans, &service_request);
  return http_client_.Export(service_request);
}

bool OtlpHttpExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return http_client_.Shutdown(timeout);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
