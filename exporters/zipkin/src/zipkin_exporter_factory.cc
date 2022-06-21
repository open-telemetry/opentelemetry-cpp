// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/zipkin/zipkin_exporter.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_options.h"
#include "opentelemetry/exporters/zipkin/zipkin_exporter_factory.h"

namespace http_client = opentelemetry::ext::http::client;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinExporterFactory::Build()
{
  ZipkinExporterOptions options;
  return Build(options);
}

nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> ZipkinExporterFactory::Build(
    const ZipkinExporterOptions &options)
{
  nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter(new ZipkinExporter(options));
  return exporter;
}

}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
