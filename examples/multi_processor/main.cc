// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

// Using an exporter that simply dumps span data to stdout.
#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif
#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/exporters/ostream/span_exporter.h"

using opentelemetry::exporter::memory::InMemorySpanExporter;
namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd     = opentelemetry::nostd;

namespace
{
InMemorySpanExporter *initTracer()
{
  auto exporter1 = std::unique_ptr<trace_sdk::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor1 = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::move(exporter1)));

  InMemorySpanExporter *memory_span_exporter = new InMemorySpanExporter();
  auto exporter2 = std::unique_ptr<trace_sdk::SpanExporter>(memory_span_exporter);

  // fetch the exporter for dumping data later

  auto processor2 = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::move(exporter2)));

  auto provider = nostd::shared_ptr<trace_sdk::TracerProvider>(
      new trace_sdk::TracerProvider(std::move(processor1)));
  provider->AddProcessor(std::move(processor2));
  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(std::move(provider));

  return memory_span_exporter;
}

void dumpSpans(std::vector<std::unique_ptr<trace_sdk::SpanData>> &spans)
{
  char span_buf[trace_api::SpanId::kSize * 2];
  char trace_buf[trace_api::TraceId::kSize * 2];
  char parent_span_buf[trace_api::SpanId::kSize * 2];
  std::cout << "\nSpans from memory :" << std::endl;

  for (auto &span : spans)
  {
    std::cout << "\n\tSpan: " << std::endl;
    std::cout << "\t\tName: " << span->GetName() << std::endl;
    span->GetSpanId().ToLowerBase16(span_buf);
    span->GetTraceId().ToLowerBase16(trace_buf);
    span->GetParentSpanId().ToLowerBase16(parent_span_buf);
    std::cout << "\t\tTraceId: " << std::string(trace_buf, sizeof(trace_buf)) << std::endl;
    std::cout << "\t\tSpanId: " << std::string(span_buf, sizeof(span_buf)) << std::endl;
    std::cout << "\t\tParentSpanId: " << std::string(parent_span_buf, sizeof(parent_span_buf))
              << std::endl;

    std::cout << "\t\tDescription: " << span->GetDescription() << std::endl;
    std::cout << "\t\tSpan kind:"
              << static_cast<typename std::underlying_type<trace_api::SpanKind>::type>(
                     span->GetSpanKind())
              << std::endl;
    std::cout << "\t\tSpan Status: "
              << static_cast<typename std::underlying_type<trace_api::StatusCode>::type>(
                     span->GetStatus())
              << std::endl;
  }
}
}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  InMemorySpanExporter *memory_span_exporter = initTracer();

  foo_library();
  auto memory_spans = memory_span_exporter->GetData()->GetSpans();
  dumpSpans(memory_spans);
}
