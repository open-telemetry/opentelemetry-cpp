// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
// Using an exporter that simply dumps span data to stdout.
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"

#include <chrono>
#include <thread>

constexpr int kNumSpans  = 10;
namespace trace_api      = opentelemetry::trace;
namespace resource       = opentelemetry::sdk::resource;
namespace exporter_trace = opentelemetry::exporter::trace;
namespace trace_sdk      = opentelemetry::sdk::trace;
namespace nostd          = opentelemetry::nostd;

namespace
{

void initTracer()
{
  auto exporter = std::unique_ptr<trace_sdk::SpanExporter>(new exporter_trace::OStreamSpanExporter);

  // CONFIGURE BATCH SPAN PROCESSOR PARAMETERS

  trace_sdk::BatchSpanProcessorOptions options{};
  // We make the queue size `KNumSpans`*2+5 because when the queue is half full, a preemptive notif
  // is sent to start an export call, which we want to avoid in this simple example.
  options.max_queue_size = kNumSpans * 2 + 5;
  // Time interval (in ms) between two consecutive exports.
  options.schedule_delay_millis = std::chrono::milliseconds(3000);
  // We export `kNumSpans` after every `schedule_delay_millis` milliseconds.
  options.max_export_batch_size = kNumSpans;

  resource::ResourceAttributes attributes = {{"service", "test_service"}, {"version", (uint32_t)1}};
  auto resource                           = resource::Resource::Create(attributes);

  auto processor = std::unique_ptr<trace_sdk::SpanProcessor>(
      new trace_sdk::BatchSpanProcessor(std::move(exporter), options));

  auto provider = nostd::shared_ptr<trace_api::TracerProvider>(
      new trace_sdk::TracerProvider(std::move(processor), resource));
  // Set the global trace provider.
  trace_api::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<trace_api::Tracer> get_tracer()
{
  auto provider = trace_api::Provider::GetTracerProvider();
  return provider->GetTracer("foo_library");
}

void StartAndEndSpans()
{
  for (int i = 1; i <= kNumSpans; ++i)
  {
    get_tracer()->StartSpan("Span " + std::to_string(i));
  }
}

}  // namespace

int main()
{
  // Removing this line will leave the default noop TracerProvider in place.
  initTracer();

  std::cout << "Creating first batch of " << kNumSpans << " spans and waiting 3 seconds ...\n";
  StartAndEndSpans();
  std::this_thread::sleep_for(std::chrono::milliseconds(3000 + 50));
  // The spans should now be exported.
  std::cout << "....Exported!\n\n\n";

  // Do the same again
  std::cout << "Creating second batch of " << kNumSpans << " spans and waiting 3 seconds ...\n";
  StartAndEndSpans();
  std::this_thread::sleep_for(std::chrono::milliseconds(3000 + 50));
  std::cout << "....Exported!\n\n\n";

  // Shutdown and drain queue
  StartAndEndSpans();
  printf("Shutting down and draining queue.... \n");
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  // We immediately let the program terminate which invokes the processor destructor
  // which in turn invokes the processor Shutdown(), which finally drains the queue of ALL
  // its spans.
}
