// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_http_exporter_runtime_options.h"
#include "opentelemetry/ext/http/client/http_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create()
{
  OtlpHttpExporterOptions options;
  return Create(options);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create(
    const OtlpHttpExporterOptions &options)
{
  OtlpHttpExporterRuntimeOptions runtime_options;
  return Create(options, runtime_options);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create(
    const OtlpHttpExporterOptions &options,
    const OtlpHttpExporterRuntimeOptions &runtime_options)
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter(
      new OtlpHttpExporter(options, runtime_options));
  return exporter;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create(
    const OtlpHttpExporterOptions &options,
    const std::shared_ptr<opentelemetry::ext::http::client::HttpClientFactory> &factory)
{
  return std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new OtlpHttpExporter(options, factory));
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create(
    const OtlpHttpExporterOptions &options,
    const OtlpHttpExporterRuntimeOptions &runtime_options,
    const std::shared_ptr<opentelemetry::ext::http::client::HttpClientFactory> &factory)
{
  return std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new OtlpHttpExporter(options, runtime_options, factory));
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create(
    const OtlpHttpExporterOptions &options,
    std::shared_ptr<opentelemetry::ext::http::client::HttpClient> http_client)
{
  return std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new OtlpHttpExporter(options, std::move(http_client)));
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> OtlpHttpExporterFactory::Create(
    const OtlpHttpExporterOptions &options,
    const OtlpHttpExporterRuntimeOptions &runtime_options,
    std::shared_ptr<opentelemetry::ext::http::client::HttpClient> http_client)
{
  return std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new OtlpHttpExporter(options, runtime_options, std::move(http_client)));
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
