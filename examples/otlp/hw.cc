#include <stdio.h>

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_http_exporter.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

namespace
{
opentelemetry::exporter::otlp::OtlpHttpExporterOptions opts;

void InitTracer()
{
  opts.url = "http://localhost:4318/v1/traces";

  namespace trace     = opentelemetry::trace;
  namespace nostd     = opentelemetry::nostd;
  namespace trace_sdk = opentelemetry::sdk::trace;
  namespace otlp      = opentelemetry::exporter::otlp;

  // Create OTLP exporter instance
  auto exporter  = std::unique_ptr<trace_sdk::SpanExporter>(new otlp::OtlpHttpExporter(opts));
  auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new trace_sdk::TracerProvider(std::move(processor)));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main() {
  InitTracer();

  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
  auto tracer = provider->GetTracer("bee_backend", "1.0.0");
  auto span = tracer->StartSpan("bee_backend");
  auto scope = tracer->WithActiveSpan(span);

  printf("hello\n");

  span->End();

  return 0;
}
