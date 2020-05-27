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
  // Specify the exporter: StdoutExporter in this case simply passes all output from the span processor to stdout
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new StdoutExporter);
  // Specify the Span Processor: SimpleSpanProcessor forwards all completed spans directly to the exporter
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter))); //Creating a Span Processor given the StdoutExporter defined earlier
  // Initialize a trace provider with the Span Processor instance defined above
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor));
  // Set global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{
  // Removing this line will leave OT initialized with the default noop
  // tracer, thus being effectively deactivated.
  initTracer();

  // Call the instrumented library
  foo_library();
}
