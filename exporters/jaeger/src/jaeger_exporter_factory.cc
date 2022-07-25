// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter_factory.h>
#include <opentelemetry/exporters/jaeger/jaeger_exporter_options.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> JaegerExporterFactory::Create()
{
  JaegerExporterOptions options;
  return Create(options);
}

std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> JaegerExporterFactory::Create(
    const JaegerExporterOptions &options)
{
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter(new JaegerExporter(options));
  return exporter;
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
