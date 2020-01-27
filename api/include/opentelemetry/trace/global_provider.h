#pragma once

#include <atomic>

#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"

using opentelemetry::trace::NoopTracer;
using opentelemetry::trace::TracerProvider;

namespace opentelemetry
{
namespace trace
{

class DefaultTracerProvider final : public TracerProvider
{
public:
  NoopTracer *const GetTracer(string_view library_name, string_view library_version = "") override
  {
    return tracer_.get();
  }

private:
  DefaultTracerProvider() : tracer_{nostd::unique_ptr<NoopTracer>(new NoopTracer)} {}
  nostd::unique_ptr<NoopTracer> tracer_;

  friend class Provider;
};

class Provider
{
public:
public:
  static TracerProvider *GetTracerProvider() { return Ptr()->load(std::memory_order_acquire); }

  static void SetTracerProvider(TracerProvider *tp)
  {
    if (auto old = Ptr()->load(std::memory_order_acquire))
    {
      delete old;
    }
    Ptr()->store(tp, std::memory_order_release);
  }

private:
  static std::atomic<TracerProvider *> *Ptr()
  {
    static std::atomic<TracerProvider *> *singleton =
        new std::atomic<TracerProvider *>(new DefaultTracerProvider);
    return singleton;
  }
};
}  // namespace trace
}  // namespace opentelemetry
