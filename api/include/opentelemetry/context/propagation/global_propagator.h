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

class PropagatorSingleton
{
public:
  PropagatorSingleton() : lock(), propagator(new NoOpPropagator()) {}

  common::SpinLockMutex lock;
  nostd::shared_ptr<TextMapPropagator> propagator;

  PropagatorSingleton(const PropagatorSingleton &) = delete;
  PropagatorSingleton &operator=(const PropagatorSingleton &) = delete;
  PropagatorSingleton(PropagatorSingleton &&)                 = delete;
  PropagatorSingleton &operator=(PropagatorSingleton &&) = delete;
};

/**
 * Stores the singleton TextMapPropagator.
 */
class GlobalTextMapPropagator
{
public:
  static nostd::shared_ptr<TextMapPropagator> GetGlobalPropagator() noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(s.lock);
    nostd::shared_ptr<TextMapPropagator> result(s.propagator);
    return result;
  }

  static void SetGlobalPropagator(nostd::shared_ptr<TextMapPropagator> prop) noexcept
  {
    std::lock_guard<common::SpinLockMutex> guard(s.lock);
    s.propagator = prop;
  }

private:
  OPENTELEMETRY_DECLARE_API_SINGLETON static PropagatorSingleton s;
};

OPENTELEMETRY_DEFINE_API_SINGLETON PropagatorSingleton GlobalTextMapPropagator::s;

}  // namespace propagation
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
