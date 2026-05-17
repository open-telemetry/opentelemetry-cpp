// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "opentelemetry/exporters/zipkin/zipkin_exporter.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_factory.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/version.h"

namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinExporterFactory::Create()
{
  ZipkinExporterOptions options;
  return Create(options);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinExporterFactory::Create(
    const ZipkinExporterOptions &options)
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter(new ZipkinExporter(options));
  return exporter;
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinExporterFactory::Create(
    const ZipkinExporterOptions &options,
    std::shared_ptr<opentelemetry::ext::http::client::HttpClientFactory> factory)
{
  return std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new ZipkinExporter(options, std::move(factory)));
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinExporterFactory::Create(
    const ZipkinExporterOptions &options,
    std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> http_client)
{
  return std::unique_ptr<opentelemetry::sdk::trace::SpanExporter>(
      new ZipkinExporter(options, std::move(http_client)));
}

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
