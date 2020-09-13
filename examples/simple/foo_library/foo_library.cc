#include "opentelemetry/trace/provider.h"

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

namespace
{
nostd::shared_ptr<trace::Tracer> get_tracer()
{
  auto provider = trace::Provider::GetTracerProvider();
  return provider->GetTracer("foo_library");
}

void f1()
{
  auto span = get_tracer()->StartSpan("f1");
  auto scope = get_tracer()->WithActiveSpan(span);
}

void f2()
{
  auto span = get_tracer()->StartSpan("f2");
  auto scope = get_tracer()->WithActiveSpan(span);

  f1();
  f1();
}
}  // namespace

void foo_library()
{
  auto span = get_tracer()->StartSpan("library");
  auto scope = get_tracer()->WithActiveSpan(span);

  f2();
}
