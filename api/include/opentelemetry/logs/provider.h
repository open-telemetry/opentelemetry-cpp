/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/logs/logger_provider.h"
#include "opentelemetry/logs/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"

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
  static nostd::shared_ptr<LoggerProvider> &GetProvider() noexcept
  {
    static nostd::shared_ptr<LoggerProvider> provider(new NoopLoggerProvider);
    return provider;
  }

  static common::SpinLockMutex &GetLock() noexcept
  {
    static common::SpinLockMutex lock;
    return lock;
  }
};

}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
