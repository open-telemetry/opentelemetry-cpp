// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
// The forward declaration is sufficient,
// we do not want a API -> SDK dependency.
// IWYU pragma: no_include "opentelemetry/sdk/trace/provider.h"
class Provider;  // IWYU pragma: keep
}  // namespace trace
}  // namespace sdk

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

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
private:
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

  /**
   * Changes the singleton TracerProvider.
   */
  static void SetTracerProvider(const nostd::shared_ptr<TracerProvider> &tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    GetProvider() = tp;
  }

private:
  /* The SDK is allowed to change the singleton in the API. */
  friend class opentelemetry::sdk::trace::Provider;

  OPENTELEMETRY_API_SINGLETON static nostd::shared_ptr<TracerProvider> &GetProvider() noexcept
  {
    static nostd::shared_ptr<TracerProvider> provider(new NoopTracerProvider);
    return provider;
  }

  OPENTELEMETRY_API_SINGLETON static common::SpinLockMutex &GetLock() noexcept
  {
    static common::SpinLockMutex lock;
    return lock;
  }
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
