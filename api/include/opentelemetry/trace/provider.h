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
    std::lock_guard<common::SpinLockMutex> guard(lock);
    nostd::shared_ptr<TracerProvider> result(provider);
    return result;
  }

  /**
   * Changes the singleton TracerProvider.
   */
  static void SetTracerProvider(nostd::shared_ptr<TracerProvider> tp) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(lock);
    provider = tp;
  }

private:
  OPENTELEMETRY_DECLARE_EXPORT static nostd::shared_ptr<TracerProvider> provider;

  OPENTELEMETRY_DECLARE_EXPORT static common::SpinLockMutex lock;
};

OPENTELEMETRY_DEFINE_EXPORT nostd::shared_ptr<TracerProvider> Provider::provider(
    new NoopTracerProvider);

OPENTELEMETRY_DEFINE_EXPORT common::SpinLockMutex Provider::lock;

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
