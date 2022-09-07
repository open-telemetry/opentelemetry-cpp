// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/common/macros.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"
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
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    return nostd::shared_ptr<TracerProvider>(GetProvider());
  }

  /**
   * Changes the singleton TracerProvider.
   */
  static void SetTracerProvider(nostd::shared_ptr<TracerProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    GetProvider() = tp;
  }

private:
  OPENTELEMETRY_API_SINGLETON static nostd::shared_ptr<TracerProvider> &GetProvider() noexcept
  {
#ifdef OPENTELEMETRY_SINGLETON_IN_METHOD
    static nostd::shared_ptr<TracerProvider> provider(new NoopTracerProvider);
#endif
    return provider;
  }

  OPENTELEMETRY_API_SINGLETON static common::SpinLockMutex &GetLock() noexcept
  {
#ifdef OPENTELEMETRY_SINGLETON_IN_METHOD
    static common::SpinLockMutex lock;
#endif
    return lock;
  }

#ifdef OPENTELEMETRY_SINGLETON_IN_MEMBER
  static nostd::shared_ptr<TracerProvider> provider;
  static common::SpinLockMutex lock;
#endif
};

#ifdef OPENTELEMETRY_SINGLETON_IN_MEMBER
OPENTELEMETRY_MEMBER_SINGLETON nostd::shared_ptr<TracerProvider> Provider::provider(
    new NoopTracerProvider);
OPENTELEMETRY_MEMBER_SINGLETON common::SpinLockMutex Provider::lock;
#endif

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
