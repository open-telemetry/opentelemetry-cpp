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
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    return nostd::shared_ptr<LoggerProvider>(GetProvider());
  }

  /**
   * Changes the singleton LoggerProvider.
   */
  static void SetLoggerProvider(nostd::shared_ptr<LoggerProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(GetLock());
    GetProvider() = tp;
  }

private:
  OPENTELEMETRY_API_SINGLETON static nostd::shared_ptr<LoggerProvider> &GetProvider() noexcept
  {
#  ifdef OPENTELEMETRY_SINGLETON_IN_METHOD
    static nostd::shared_ptr<LoggerProvider> provider(new NoopLoggerProvider);
#  endif
    return provider;
  }

  OPENTELEMETRY_API_SINGLETON static common::SpinLockMutex &GetLock() noexcept
  {
#  ifdef OPENTELEMETRY_SINGLETON_IN_METHOD
    static common::SpinLockMutex lock;
#  endif
    return lock;
  }

#  ifdef OPENTELEMETRY_SINGLETON_IN_MEMBER
  static nostd::shared_ptr<LoggerProvider> provider;
  static common::SpinLockMutex lock;
#  endif
};

#  ifdef OPENTELEMETRY_SINGLETON_IN_MEMBER
OPENTELEMETRY_MEMBER_SINGLETON nostd::shared_ptr<LoggerProvider> Provider::provider(
    new NoopLoggerProvider);
OPENTELEMETRY_MEMBER_SINGLETON common::SpinLockMutex Provider::lock;
#  endif

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
