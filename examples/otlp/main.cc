#include "opentelemetry/exporters/otlp/otlp_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

#include "foo_library/foo_library.h"

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;
namespace otlp     = opentelemetry::exporter::otlp;

namespace
{
opentelemetry::exporter::otlp::OtlpExporterOptions opts;
void InitTracer()
{
  // Create OTLP exporter instance
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(new otlp::OtlpExporter(opts));

  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    opts.endpoint = argv[1];
  }
  // Removing this line will leave the default noop TracerProvider in place.
  InitTracer();

  foo_library();
}
