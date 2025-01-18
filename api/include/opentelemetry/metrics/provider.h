// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
// The forward declaration is sufficient,
// we do not want a API -> SDK dependency.
// IWYU pragma: no_include "opentelemetry/sdk/metrics/provider.h"
class Provider;  // IWYU pragma: keep
}  // namespace metrics
}  // namespace sdk

namespace metrics
{

class MeterProvider;

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
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    return nostd::shared_ptr<MeterProvider>(GetProvider());
  }

#if OPENTELEMETRY_ABI_VERSION_NO == 1

  /**
   * Changes the singleton MeterProvider.
   */
  static void SetMeterProvider(const nostd::shared_ptr<MeterProvider> &tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    GetProvider() = tp;
  }

#endif /* OPENTELEMETRY_ABI_VERSION_NO */

private:
  /* The SDK is allowed to change the singleton in the API. */
  friend class opentelemetry::sdk::metrics::Provider;

  OPENTELEMETRY_API_SINGLETON static nostd::shared_ptr<MeterProvider> &GetProvider() noexcept
  {
    static nostd::shared_ptr<MeterProvider> provider(new NoopMeterProvider);
    return provider;
  }

  OPENTELEMETRY_API_SINGLETON static common::SpinLockMutex &GetLock() noexcept
  {
    static common::SpinLockMutex lock;
    return lock;
  }
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
