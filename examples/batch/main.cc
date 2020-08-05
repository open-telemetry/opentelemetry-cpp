#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
// Using an exporter that simply dumps span data to stdout.
#include "opentelemetry/exporters/ostream/span_exporter.h"
#include "opentelemetry/sdk/trace/batch_span_processor.h"

#include <chrono>
#include <thread>

constexpr int kNumSpans = 10;

namespace
{

void initTracer()
{
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);

  // CONFIGURE BATCH SPAN PROCESSOR PARAMETERS

  // We make the queue size `KNumSpans`*2+5 because when the queue is half full, a preemptive notif
  // is sent to start an export call, which we want to avoid in this simple example.
  const size_t max_queue_size = kNumSpans * 2 + 5;

  // Time interval (in ms) between two consecutive exports.
  const std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(3000);

  // We export `kNumSpans` after every `schedule_delay_millis` milliseconds.
  const size_t max_export_batch_size = kNumSpans;

  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(new sdktrace::BatchSpanProcessor(
      std::move(exporter), max_queue_size, schedule_delay_millis, max_export_batch_size));

  auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(processor));
  // Set the global trace provider.
  opentelemetry::trace::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer()
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
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
