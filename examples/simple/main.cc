#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

// Using an exporter that simply dumps span data to stdout.
#include "stdout_exporter.h"

// This models a call to an external library that is instrumented with OT.
extern void library();

void initTracer()
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new StdoutExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor));
  trace::Provider::SetTracerProvider(provider);
}

int main()
{
  // Removing this line will leave OT initialized with the default noop
  // tracer, thus being effectively deactivated.
  initTracer();

  library();
}
