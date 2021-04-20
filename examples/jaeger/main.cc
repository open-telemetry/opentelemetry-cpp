#include "opentelemetry/exporters/jaeger/jaeger_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "foo_library/foo_library.h"

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;
namespace jaeger   = opentelemetry::exporter::jaeger;

namespace
{
opentelemetry::exporter::jaeger::JaegerExporterOptions opts;
void InitTracer()
{
  // Create Jaeger exporter instance
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new jaeger::JaegerExporter(opts));
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(std::move(processor)));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    opts.server_addr = argv[1];
  }
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();
}
