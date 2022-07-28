// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mutex>

#include "opentelemetry/context/propagation/noop_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"

#include "opentelemetry/common/macros.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/shared_ptr.h"

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
namespace propagation
{

/**
 * Stores the singleton TextMapPropagator.
 */
class GlobalTextMapPropagator
{
public:
  static nostd::shared_ptr<TextMapPropagator> GetGlobalPropagator() noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(lock);
    nostd::shared_ptr<TextMapPropagator> result(propagator);
    return result;
  }

  static void SetGlobalPropagator(nostd::shared_ptr<TextMapPropagator> prop) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(lock);
    propagator = prop;
  }

private:
  static nostd::shared_ptr<TextMapPropagator> propagator;

  static common::SpinLockMutex lock;
};

OPENTELEMETRY_EXPORT nostd::shared_ptr<TextMapPropagator> GlobalTextMapPropagator::propagator(
    new NoOpPropagator());

OPENTELEMETRY_EXPORT common::SpinLockMutex GlobalTextMapPropagator::lock;

}  // namespace propagation
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
