// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_log_record_exporter_runtime_options.h"
#include "opentelemetry/ext/http/client/http_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpHttpLogRecordExporterFactory::Create()
{
  OtlpHttpLogRecordExporterOptions options;
  return Create(options);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpHttpLogRecordExporterFactory::Create(const OtlpHttpLogRecordExporterOptions &options)
{
  OtlpHttpLogRecordExporterRuntimeOptions runtime_options;
  return Create(options, runtime_options);
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpHttpLogRecordExporterFactory::Create(
    const OtlpHttpLogRecordExporterOptions &options,
    const OtlpHttpLogRecordExporterRuntimeOptions &runtime_options)
{
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter(
      new OtlpHttpLogRecordExporter(options, runtime_options));
  return exporter;
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpHttpLogRecordExporterFactory::Create(
    const OtlpHttpLogRecordExporterOptions &options,
    std::shared_ptr<opentelemetry::ext::http::client::HttpClientFactory> factory)
{
  return std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>(
      new OtlpHttpLogRecordExporter(options, std::move(factory)));
}

std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>
OtlpHttpLogRecordExporterFactory::Create(
    const OtlpHttpLogRecordExporterOptions &options,
    std::shared_ptr<opentelemetry::ext::http::client::HttpClient> http_client)
{
  return std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter>(
      new OtlpHttpLogRecordExporter(options, std::move(http_client)));
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
