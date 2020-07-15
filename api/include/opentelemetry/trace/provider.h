#pragma once

#include <atomic>

#include "opentelemetry/nostd/nostd.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
/**
 * Stores the singleton global TracerProvider.
 */
class Provider
{
public:
  /**
   * Returns the singleton TracerProvider.
   *
   * By default, a no-op TracerProvider is returned. This will never return a
   * nullptr TracerProvider.
   */
  static nostd::shared_ptr<TracerProvider> GetTracerProvider() noexcept
  {
    while (GetLock().test_and_set(std::memory_order_acquire))
      ;
    auto provider = nostd::shared_ptr<TracerProvider>(GetProvider());
    GetLock().clear(std::memory_order_release);

    return provider;
  }

  /**
   * Changes the singleton TracerProvider.
   */
  static void SetTracerProvider(nostd::shared_ptr<TracerProvider> tp) noexcept
  {
    while (GetLock().test_and_set(std::memory_order_acquire))
      ;
    GetProvider() = tp;
    GetLock().clear(std::memory_order_release);
  }

private:
  static nostd::shared_ptr<TracerProvider> &GetProvider() noexcept
  {
    static nostd::shared_ptr<TracerProvider> provider(new NoopTracerProvider);
    return provider;
  }

  static std::atomic_flag &GetLock() noexcept
  {
    static std::atomic_flag lock = ATOMIC_FLAG_INIT;
    return lock;
  }
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
