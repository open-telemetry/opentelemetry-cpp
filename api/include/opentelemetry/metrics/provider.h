#pragma once

#include <atomic>

#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{
/**
 * Stores the singleton global MeterProvider.
 */
class Provider
{
public:
  /**
   * Returns the singleton MeterProvider.
   *
   * By default, a no-op MeterProvider is returned. This will never return a
   * nullptr MeterProvider.
   */
  static nostd::shared_ptr<MeterProvider> GetMeterProvider() noexcept
  {
    while (GetLock().test_and_set(std::memory_order_acquire))
      ;
    auto provider = nostd::shared_ptr<MeterProvider>(GetProvider());
    GetLock().clear(std::memory_order_release);

    return provider;
  }

  /**
   * Changes the singleton MeterProvider.
   */
  static void SetMeterProvider(nostd::shared_ptr<MeterProvider> tp) noexcept
  {
    while (GetLock().test_and_set(std::memory_order_acquire))
      ;
    GetProvider() = tp; 
    GetLock().clear(std::memory_order_release);
  }

private:
  static nostd::shared_ptr<MeterProvider> &GetProvider() noexcept
  {
    static nostd::shared_ptr<MeterProvider> provider(new NoopMeterProvider);
    return provider;
  }

  static std::atomic_flag &GetLock() noexcept
  {
    static std::atomic_flag lock = ATOMIC_FLAG_INIT;
    return lock;
  }
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
