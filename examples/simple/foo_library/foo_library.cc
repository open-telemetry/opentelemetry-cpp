#include "opentelemetry/trace/provider.h"

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

namespace
{
nostd::shared_ptr<trace::Tracer> get_tracer()
{
  // Retrieve the global TraceProvider specific in main
  auto provider = trace::Provider::GetTracerProvider();
  // Return a Tracer object
  return provider->GetTracer("foo_library");
}

void f1()
{
  // Create a span using the Tracer returned by the get_tracer() function
  auto span = get_tracer()->StartSpan("f1");
}

void f2()
{
  auto span = get_tracer()->StartSpan("f2");

  f1();
  f1();
}
}  // namespace

void foo_library()
{
  auto span = get_tracer()->StartSpan("library");

  f2();
}
