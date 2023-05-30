// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/version.h"

#define BUILD_COMPONENT_E

#include "component_e.h"

namespace trace = opentelemetry::trace;
namespace nostd = opentelemetry::nostd;

static nostd::shared_ptr<trace::Tracer> get_tracer()
{
  auto provider = trace::Provider::GetTracerProvider();
  return provider->GetTracer("H", "80.8");
}

static void f1()
{
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("H::f1"));
}

static void f2()
{
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("H::f2"));

  f1();
  f1();
}


#if defined(_MSC_VER)
// component_g is a DLL

__declspec(dllexport)

#else
// component_g is a shared library (*.so)
// component_g is compiled with visibility("hidden"),
__attribute__((visibility("default")))
#endif

extern "C"
void do_something_in_h()
{
  auto scoped_span = trace::Scope(get_tracer()->StartSpan("H::library"));

  f2();
}
