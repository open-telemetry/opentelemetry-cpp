// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/context/propagation/noop_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"

#include "opentelemetry/common/macros.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
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
  OPENTELEMETRY_API_SINGLETON static nostd::shared_ptr<TextMapPropagator> &GetPropagator() noexcept
  {
#ifdef OPENTELEMETRY_SINGLETON_IN_METHOD
    static nostd::shared_ptr<TextMapPropagator> propagator(new NoOpPropagator());
#endif
    return propagator;
  }

  OPENTELEMETRY_API_SINGLETON static common::SpinLockMutex &GetLock() noexcept
  {
#ifdef OPENTELEMETRY_SINGLETON_IN_METHOD
    static common::SpinLockMutex lock;
#endif
    return lock;
  }

#ifdef OPENTELEMETRY_SINGLETON_IN_MEMBER
  static nostd::shared_ptr<TextMapPropagator> propagator;
  static common::SpinLockMutex lock;
#endif
};

#ifdef OPENTELEMETRY_SINGLETON_IN_MEMBER
OPENTELEMETRY_MEMBER_SINGLETON nostd::shared_ptr<TextMapPropagator>
    GlobalTextMapPropagator::propagator(new NoOpPropagator());
OPENTELEMETRY_MEMBER_SINGLETON common::SpinLockMutex GlobalTextMapPropagator::lock;
#endif

}  // namespace propagation
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
