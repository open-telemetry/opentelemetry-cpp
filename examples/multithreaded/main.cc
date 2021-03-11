#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/scope.h"

// Using an exporter that simply dumps span data to stdout.
#include "opentelemetry/exporters/ostream/span_exporter.h"

#include <algorithm>
#include <thread>

namespace
{
void initTracer()
{
  auto exporter = std::unique_ptr<sdktrace::SpanExporter>(
      new opentelemetry::exporter::trace::OStreamSpanExporter);
  auto processor = std::unique_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider = nostd::shared_ptr<opentelemetry::trace::TracerProvider>(
      new sdktrace::TracerProvider(std::move(processor), opentelemetry::sdk::resource::Resource::Create({})));
  // Set the global trace provider
  opentelemetry::trace::Provider::SetTracerProvider(provider);
}

nostd::shared_ptr<opentelemetry::trace::Tracer> get_tracer()
{
  auto provider = opentelemetry::trace::Provider::GetTracerProvider();
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
      opentelemetry::trace::Scope scope(thread_span);
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
  opentelemetry::trace::Scope scope(root_span);

  run_threads();
}
