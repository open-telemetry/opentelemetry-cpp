// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/version.h"

#ifdef _WIN32
# include "opentelemetry/common/detail/symbol_bridge_windows.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

/**
 * Stores the singleton global TracerProvider.
 */
class OPENTELEMETRY_EXPORT Provider
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
   *
   * Passing a nullptr TracerProvider installs a no-op TracerProvider, so that
   * GetTracerProvider() never returns a nullptr TracerProvider.
   */
  static void SetTracerProvider(const nostd::shared_ptr<TracerProvider> &tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    if (tp)
    {
      GetProvider() = tp;
    }
    else
    {
      GetProvider() = nostd::shared_ptr<TracerProvider>(new NoopTracerProvider);
    }
  }

private:
  OPENTELEMETRY_API_SINGLETON static nostd::shared_ptr<TracerProvider> &GetProvider() noexcept
  {
#ifdef _WIN32
    if (const auto address = common::detail::LoadSymbolBridgeSymbol<nostd::shared_ptr<TracerProvider>& (*)()>("OpenTelemetryTraceProviderGetProvider"))
      return address();
#endif
    static nostd::shared_ptr<TracerProvider> provider(new NoopTracerProvider);
    return provider;
  }

  OPENTELEMETRY_API_SINGLETON static common::SpinLockMutex &GetLock() noexcept
  {
#ifdef _WIN32
    if (const auto address = common::detail::LoadSymbolBridgeSymbol<common::SpinLockMutex& (*)(), TracerProvider>("OpenTelemetryTraceProviderGetLock"))
      return address();
#endif
    static common::SpinLockMutex lock;
    return lock;
  }
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
