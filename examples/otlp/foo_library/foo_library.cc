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
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("f1"));
}

void f2()
{
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("f2"));

  f1();
  f1();
}
}  // namespace

void foo_library()
{
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("library"));

  f2();
}
