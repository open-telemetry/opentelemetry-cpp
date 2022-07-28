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
    std::lock_guard<common::SpinLockMutex> guard(lock);
    nostd::shared_ptr<LoggerProvider> result(provider);
    return result;
  }

  /**
   * Changes the singleton LoggerProvider.
   */
  static void SetLoggerProvider(nostd::shared_ptr<LoggerProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(lock);
    provider = tp;
  }

private:
  static nostd::shared_ptr<LoggerProvider> provider;

  static common::SpinLockMutex lock;
};

OPENTELEMETRY_EXPORT nostd::shared_ptr<LoggerProvider> Provider::provider(new NoopLoggerProvider);

OPENTELEMETRY_EXPORT common::SpinLockMutex Provider::lock;

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
#endif
