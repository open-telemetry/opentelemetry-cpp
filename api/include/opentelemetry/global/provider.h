#pragma once

#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/nostd/unique_ptr.h"

using opentelemetry::trace::NoopTracer;
using opentelemetry::trace::TracerProvider;

namespace opentelemetry
{
namespace global
{

class DefaultTracerProvider final : public TracerProvider
{
public:
  explicit DefaultTracerProvider() : tracer_{new NoopTracer()} {}
  ~DefaultTracerProvider() { delete tracer_; }

  NoopTracer *const GetTracer(string_view library_name, string_view library_version = "") override
  {
    return tracer_;
  }

private:
  NoopTracer *const tracer_;
};

class Provider
{
public:
  static TracerProvider *GetTracerProvider()
  {
    auto ref_ptr = GetRefPtr();
    auto ref = *ref_ptr;
    if (!ref)
    {
      ref = new DefaultTracerProvider();
      *ref_ptr = ref;
    }

    return ref;
  }

  static void SetTracerProvider(TracerProvider *tp)
  {
    auto ref_ptr = GetRefPtr();
    auto ref = *ref_ptr;
    if (ref)
    {
      delete ref;
    }

    *ref_ptr = tp;
  }
private:
  static TracerProvider **GetRefPtr()
  {
    static TracerProvider *tp = nullptr;
    return &tp;
  }
};
}  // namespace global
}  // namespace opentelemetry
