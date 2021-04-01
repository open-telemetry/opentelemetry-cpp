#pragma once

#include <mutex>

#include "opentelemetry/trace/propagation/noop_propagator.h"
#include "opentelemetry/trace/propagation/text_map_propagator.h"

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

/* Stores the singlaton TextMapPropagator */

template <typename T>
class GlobalPropagator
{
public:
  static nostd::shared_ptr<TextMapPropagator<T>> GetGlobalPropagator() noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    return nostd::shared_ptr<TextMapPropagator<T>>(GetPropagator());
  }

  static void SetGlobalPropagator(nostd::shared_ptr<TextMapPropagator<T>> prop) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    GetPropagator() = prop;
  }

private:
  static nostd::shared_ptr<TextMapPropagator<T>> &GetPropagator() noexcept
  {
    static nostd::shared_ptr<TextMapPropagator<T>> propagators(new NoOpPropagator<T>());
    return propagators;
  }

  static common::SpinLockMutex &GetLock() noexcept
  {
    static common::SpinLockMutex lock;
    return lock;
  }
};

}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE