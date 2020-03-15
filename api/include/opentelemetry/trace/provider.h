#pragma once

#include <atomic>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

class DefaultTracerProvider final : public opentelemetry::trace::TracerProvider
{
public:
  nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version) override
  {
    return nostd::shared_ptr<opentelemetry::trace::Tracer>(tracer_);
  }

private:
  DefaultTracerProvider()
      : tracer_{nostd::shared_ptr<opentelemetry::trace::NoopTracer>(
            new opentelemetry::trace::NoopTracer)}
  {}
  nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;

  friend class Provider;
};

class Provider
{
public:
  static nostd::shared_ptr<TracerProvider> GetTracerProvider() noexcept
  {
    nostd::shared_ptr<TracerProvider> *ptr = GetProvider().load();

    if (nullptr == ptr)
    {
      SetTracerProvider(nostd::shared_ptr<TracerProvider>(new DefaultTracerProvider));

      ptr = GetProvider().load();
    }

    return nostd::shared_ptr<TracerProvider>(*ptr);
  }

  static void SetTracerProvider(nostd::shared_ptr<TracerProvider> tp) noexcept
  {
    delete GetProvider().load();

    GetProvider().exchange(new nostd::shared_ptr<TracerProvider>(tp), std::memory_order_acq_rel);
  }

  static void SetTracerProvider(std::nullptr_t) noexcept
  {
    delete GetProvider().load();

    GetProvider().exchange(nullptr, std::memory_order_acq_rel);
  }

private:
  static std::atomic<nostd::shared_ptr<TracerProvider> *> &GetProvider() noexcept
  {
    static std::atomic<nostd::shared_ptr<TracerProvider> *> provider(
        new nostd::shared_ptr<TracerProvider>(new DefaultTracerProvider));
    return provider;
  }
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
