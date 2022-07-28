// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include <mutex>

#  include "opentelemetry/common/macros.h"
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/metrics/meter_provider.h"
#  include "opentelemetry/metrics/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"

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
    std::lock_guard<common::SpinLockMutex> guard(lock);
    nostd::shared_ptr<MeterProvider> result(provider);
    return result;
  }

  /**
   * Changes the singleton MeterProvider.
   */
  static void SetMeterProvider(nostd::shared_ptr<MeterProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(lock);
    provider = tp;
  }

private:
  static nostd::shared_ptr<MeterProvider> provider;

  static common::SpinLockMutex lock;
};

OPENTELEMETRY_EXPORT nostd::shared_ptr<MeterProvider> Provider::provider(new NoopMeterProvider);

OPENTELEMETRY_EXPORT common::SpinLockMutex Provider::lock;

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
