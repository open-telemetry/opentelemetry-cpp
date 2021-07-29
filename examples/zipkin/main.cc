// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/zipkin/zipkin_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "foo_library/foo_library.h"

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;
namespace zipkin   = opentelemetry::exporter::zipkin;

namespace
{
opentelemetry::exporter::zipkin::ZipkinExporterOptions opts;
void InitTracer()
{
  // Create zipkin exporter instance
  opentelemetry::sdk::resource::ResourceAttributes attributes = {
      {"service.name", "zipkin_demo_service"}};
  auto resource  = opentelemetry::sdk::resource::Resource::Create(attributes);
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new zipkin::ZipkinExporter(opts));
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdktrace::TracerProvider(std::move(processor), resource));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    opts.endpoint = argv[1];
  }
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();
}
