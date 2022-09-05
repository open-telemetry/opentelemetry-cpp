// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <mutex>

#  include "opentelemetry/common/macros.h"
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/logs/logger_provider.h"
#  include "opentelemetry/logs/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{

class ProviderSingleton
{
public:
  ProviderSingleton() : lock(), provider(new NoopLoggerProvider()) {}

  common::SpinLockMutex lock;
  nostd::shared_ptr<LoggerProvider> provider;

  ProviderSingleton(const ProviderSingleton &) = delete;
  ProviderSingleton &operator=(const ProviderSingleton &) = delete;
  ProviderSingleton(ProviderSingleton &&)                 = delete;
  ProviderSingleton &operator=(ProviderSingleton &&) = delete;
};

/**
 * Stores the singleton global LoggerProvider.
 */
class Provider
{
public:
  /**
   * Returns the singleton LoggerProvider.
   *
   * By default, a no-op LoggerProvider is returned. This will never return a
   * nullptr LoggerProvider.
   */
  static nostd::shared_ptr<LoggerProvider> GetLoggerProvider() noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(s.lock);
    nostd::shared_ptr<LoggerProvider> result(s.provider);
    return result;
  }

  /**
   * Changes the singleton LoggerProvider.
   */
  static void SetLoggerProvider(nostd::shared_ptr<LoggerProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(s.lock);
    s.provider = tp;
  }

private:
  OPENTELEMETRY_DECLARE_API_SINGLETON static ProviderSingleton s;
};

OPENTELEMETRY_DEFINE_API_SINGLETON ProviderSingleton Provider::s;

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
