// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/exporters/memory/in_memory_span_exporter_factory.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/trace/provider.h"

#ifdef BAZEL_BUILD
#  include "examples/common/foo_library/foo_library.h"
#else
#  include "foo_library/foo_library.h"
#endif

using opentelemetry::exporter::memory::InMemorySpanData;
namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;

namespace
{
std::shared_ptr<InMemorySpanData> InitTracer()
{
  std::shared_ptr<InMemorySpanData> data;

  auto exporter1  = opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create();
  auto processor1 = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter1));

  auto exporter2  = opentelemetry::exporter::memory::InMemorySpanExporterFactory::Create(data);
  auto processor2 = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter2));

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor>> processors;
  processors.push_back(std::move(processor1));
  processors.push_back(std::move(processor2));
  std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
      trace_sdk::TracerProviderFactory::Create(std::move(processors));

  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(std::move(provider));

  return data;
}

void CleanupTracer()
{
  std::shared_ptr<opentelemetry::trace::TracerProvider> none;
  trace_api::Provider::SetTracerProvider(none);
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
  std::shared_ptr<InMemorySpanData> data = InitTracer();

  foo_library();
  auto memory_spans = data->GetSpans();
  dumpSpans(memory_spans);

  CleanupTracer();
}
