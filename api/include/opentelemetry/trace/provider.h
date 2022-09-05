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

class ProviderSingleton
{
public:
  ProviderSingleton() : lock(), provider(new NoopTracerProvider()) {}

  common::SpinLockMutex lock;
  nostd::shared_ptr<TracerProvider> provider;

  ProviderSingleton(const ProviderSingleton &) = delete;
  ProviderSingleton &operator=(const ProviderSingleton &) = delete;
  ProviderSingleton(ProviderSingleton &&)                 = delete;
  ProviderSingleton &operator=(ProviderSingleton &&) = delete;
};

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
    std::lock_guard<common::SpinLockMutex> guard(s.lock);
    nostd::shared_ptr<TracerProvider> result(s.provider);
    return result;
  }

  /**
   * Changes the singleton TracerProvider.
   */
  static void SetTracerProvider(nostd::shared_ptr<TracerProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(s.lock);
    s.provider = tp;
  }

private:
  OPENTELEMETRY_DECLARE_API_SINGLETON static ProviderSingleton s;
};

OPENTELEMETRY_DEFINE_API_SINGLETON ProviderSingleton Provider::s;

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
