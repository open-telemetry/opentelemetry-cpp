// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/common/macros.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Provider;
}
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

  /**
   * Changes the singleton MeterProvider.
   * While declared in the API class opentelemetry::metrics::Provider,
   * this method is actually part of the SDK, not API.
   * An application **MUST** link with the opentelemetry-cpp SDK,
   * to install a metrics provider, when configuring OpenTelemetry.
   * An instrumented library does not need to invoke this method.
   */
  static void SetMeterProvider(const nostd::shared_ptr<MeterProvider> &mp) noexcept;

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
