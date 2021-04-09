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

/* Stores the singleton TextMapPropagator */

class GlobalTextMapPropagator
{
public:
  static nostd::shared_ptr<TextMapPropagator> GetGlobalPropagator() noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    return nostd::shared_ptr<TextMapPropagator>(GetPropagator());
  }

  static void SetGlobalPropagator(nostd::shared_ptr<TextMapPropagator> prop) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    GetPropagator() = prop;
  }

private:
  static nostd::shared_ptr<TextMapPropagator> &GetPropagator() noexcept
  {
    static nostd::shared_ptr<TextMapPropagator> propagator(new NoOpPropagator());
    return propagator;
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