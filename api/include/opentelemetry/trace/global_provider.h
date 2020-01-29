#pragma once

#include <atomic>

#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace opentelemetry
{
namespace trace
{

class DefaultTracerProvider final : public opentelemetry::trace::TracerProvider
{
public:
  opentelemetry::trace::NoopTracer *const GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version  = "",
      const TracerOptions &tracer_options = {}) override
  {
    return tracer_.get();
  }

private:
  DefaultTracerProvider()
      : tracer_{nostd::unique_ptr<opentelemetry::trace::NoopTracer>(
            new opentelemetry::trace::NoopTracer)}
  {}
  nostd::unique_ptr<opentelemetry::trace::NoopTracer> tracer_;

  friend class Provider;
};

class Provider
{
public:
  static opentelemetry::trace::TracerProvider *GetTracerProvider()
  {
    return Ptr()->load(std::memory_order_acquire);
  }

  static void SetTracerProvider(TracerProvider *tp)
  {
    if (auto old = Ptr()->exchange(tp, std::memory_order_release))
    {
      delete old;
    }
  }

private:
  static std::atomic<opentelemetry::trace::TracerProvider *> *Ptr()
  {
    static std::atomic<opentelemetry::trace::TracerProvider *> *singleton =
        new std::atomic<opentelemetry::trace::TracerProvider *>(new DefaultTracerProvider);
    return singleton;
  }
};

}  // namespace trace
}  // namespace opentelemetry
