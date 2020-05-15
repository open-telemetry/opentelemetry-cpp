#include "opentelemetry/trace/provider.h"

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

static nostd::shared_ptr<trace::Tracer> get_tracer()
{
  auto provider = trace::Provider::GetTracerProvider();
  return provider->GetTracer("");
}

static void f1()
{
  auto span = get_tracer()->StartSpan("f1");
}

static void f2()
{
  auto span = get_tracer()->StartSpan("f2");

  f1();
  f1();
}

void library()
{
  auto span = get_tracer()->StartSpan("library");

  f2();
}
