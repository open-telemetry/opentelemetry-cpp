#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/trace/provider.h"

// Using an exporter that simply dumps span data to stdout.
#include "stdout_exporter.h"

#include "foo_library/foo_library.h"

#include <memory>
#include <iostream>

// This is the 
using namespace opentelemetry::sdk::trace;

namespace
{
void initTracer(std::shared_ptr<Sampler> sampler)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new StdoutExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor, sampler));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
}  // namespace

int main()
{

  auto always_on_sampler  = std::make_shared<AlwaysOnSampler>();
  auto always_off_sampler = std::make_shared<AlwaysOffSampler>();
  // A Parent-Or-Else sampler with a delegate AlwaysOn sampler.
  // This sampler requires SpanContext, so is not yet fully functional.
  auto parent_or_else_sampler = std::make_shared<ParentOrElseSampler>(always_on_sampler);

  std::cout << "Testing AlwaysOn Sampler...\n";
  // Set the sampler to AlwaysOn
  initTracer(always_on_sampler);

  // The Tracer records every span
  foo_library();

  std::cout << "\nTesting AlwaysOff Sampler...\n";
  // Set the sampler to AlwaysOn
  initTracer(always_off_sampler);

  // The Tracer records nothing
  foo_library();
}
