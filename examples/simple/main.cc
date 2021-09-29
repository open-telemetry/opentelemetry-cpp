// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

// Using an exporter that simply dumps span data to stdout.
#include "foo_library/foo_library.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"

namespace trace_api = opentelemetry::trace;

namespace
{
void initTracer()
{
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace_api::TracerProvider>(
      new sdktrace::TracerProvider(std::move(processor)));

  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  initTracer();

  foo_library();
}
