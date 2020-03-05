#pragma once

#include <memory>
#include <mutex>

#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"

namespace opentelemetry
{
namespace trace
{

class DefaultTracerProvider final : public opentelemetry::trace::TracerProvider
{
public:
  opentelemetry::trace::NoopTracer *const GetTracer(nostd::string_view library_name,
                                                    nostd::string_view library_version) override
  {
    return tracer_.get();
  }

private:
  DefaultTracerProvider()
      : tracer_{
            std::shared_ptr<opentelemetry::trace::NoopTracer>(new opentelemetry::trace::NoopTracer)}
  {}
  std::shared_ptr<opentelemetry::trace::NoopTracer> tracer_;

  friend class Provider;
};

class Provider
{
public:
  static opentelemetry::trace::TracerProvider *GetTracerProvider()
  {
    std::lock_guard<std::mutex> guard{*GetMutex()};
    return *GetProvider();
  }

  static TracerProvider *SetTracerProvider(TracerProvider *tp)
  {
    std::lock_guard<std::mutex> guard{*GetMutex()};
    auto oldProvider = *GetProvider();
    *GetProvider()   = tp;
    return oldProvider;
  }

private:
  static std::mutex *GetMutex()
  {
    static std::mutex *provider_mutex = new std::mutex;
    return provider_mutex;
  }

  static opentelemetry::trace::TracerProvider **GetProvider()
  {
    static opentelemetry::trace::TracerProvider *provider = new DefaultTracerProvider;
    return &provider;
  }
};

}  // namespace trace
}  // namespace opentelemetry
