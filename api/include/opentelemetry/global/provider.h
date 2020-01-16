#pragma once

#include <memory>

#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"

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
  static TracerProvider *GetTracerProvider() { return getPtrRef().get(); }
  static void SetTracerProvider(TracerProvider *tp) { getPtrRef().reset(tp); }

private:
  static std::unique_ptr<TracerProvider> &getPtrRef()
  {
    static std::unique_ptr<TracerProvider> tracePtr(new DefaultTracerProvider());
    return tracePtr;
  }
};
}  // namespace global
}  // namespace opentelemetry
