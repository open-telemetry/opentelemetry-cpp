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
class Provider;  // IWYU pragma: keep
}
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

  /**
   * Changes the singleton TracerProvider.
   * While declared in the API class opentelemetry::trace::Provider,
   * this method is actually part of the SDK, not API.
   * An application **MUST** link with the opentelemetry-cpp SDK,
   * to install a tracer provider, when configuring OpenTelemetry.
   * An instrumented library does not need to invoke this method.
   */
  static void SetTracerProvider(const nostd::shared_ptr<TracerProvider> &tp) noexcept;

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
