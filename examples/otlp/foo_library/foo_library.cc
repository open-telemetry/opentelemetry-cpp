#include "opentelemetry/context/threadlocal_context.h"
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

// TODO: Remove all calls to span->End() once context memory issue is fixed
// (https://github.com/open-telemetry/opentelemetry-cpp/issues/287)

void f1()
{
  auto span = get_tracer()->StartSpan("f1");
  span->End();
}

void f2()
{
  auto span = get_tracer()->StartSpan("f2");

  f1();
  f1();
  span->End();
}
}  // namespace

void foo_library()
{
  auto span = get_tracer()->StartSpan("library");

  f2();
  span->End();
}
