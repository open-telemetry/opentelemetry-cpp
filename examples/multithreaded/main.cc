// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/scope.h"

// Using an exporter that simply dumps span data to stdout.
#include "opentelemetry/exporters/ostream/span_exporter.h"

#include <algorithm>
#include <thread>

namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd     = opentelemetry::nostd;

namespace
{
void initTracer()
{
  auto exporter = std::unique_ptr<trace_sdk::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<trace_api::TracerProvider>(new trace_sdk::TracerProvider(
      std::move(processor), opentelemetry::sdk::resource::Resource::Create({})));
  // Set the global trace provider
  trace_api::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<trace_api::Tracer> get_tracer()
{
  auto provider = trace_api::Provider::GetTracerProvider();
  return provider->GetTracer("foo_library");
}
}  // namespace

void run_threads()
{
  auto thread_span = get_tracer()->StartSpan(__func__);

  std::vector<std::thread> threads;
  for (int thread_num = 0; thread_num < 5; ++thread_num)
  {
    // This shows how one can effectively use Scope objects to correctly
    // parent spans across threads.
    threads.push_back(std::thread([=] {
      trace_api::Scope scope(thread_span);
      auto thread_span =
          get_tracer()->StartSpan(std::string("thread ") + std::to_string(thread_num));
    }));
  }

  std::for_each(threads.begin(), threads.end(), [](std::thread &th) { th.join(); });
}

int main()
{
  initTracer();

  auto root_span = get_tracer()->StartSpan(__func__);
  trace_api::Scope scope(root_span);

  run_threads();
}
