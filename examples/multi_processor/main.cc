#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

// Using an exporter that simply dumps span data to stdout.
#include "foo_library/foo_library.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"

namespace
{
void initTracer()
{
  auto exporter1 = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor1 = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter1)));

  auto exporter2 = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::memory::InMemorySpanExporter());
  auto processor2 = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter2)));

  std::vector<std::unique_ptr<sdktrace::SpanProcessor>> processors;
  processors.push_back(std::move(processor1));
  processors.push_back(std::move(processor2));
  // Default is an always-on sampler.
  auto context = std::make_shared<sdktrace::TracerContext>(std::move(processors));

  auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(context));

  // Set the global trace provider
  opentelemetry::trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  initTracer();

  foo_library();
}
