#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

// Using an exporter that simply dumps span data to stdout.
#include "stdout_exporter.h"

#include "foo_library/foo_library.h"

namespace
{
void initTracer()
{
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(new StdoutExporter);
  // Specify the Span Processor: SimpleSpanProcessor forwards spans directly to the exporter
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{
  // Removing this line will leave OTel SDK initialized with the default noop
  // tracer, thus being effectively deactivated.
  initTracer();

  // Call the instrumented library
  foo_library();
}
