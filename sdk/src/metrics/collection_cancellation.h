// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/**
 * Internal to the metrics SDK, not installed.
 *
 * Lets the SDK's own collection code (MetricCollector and ObservableRegistry) learn that the
 * reader running the current collection has stopped waiting for it, so it can stop between
 * meters and between callbacks instead of running every remaining one.
 *
 * The state is per thread: a reader publishes its stop flag with a Scope on the thread that runs
 * the collection, and IsRequested() is false on any other thread.
 */
class CollectionCancellation
{
public:
  /**
   * @return true if a Scope is active on the calling thread and its stop flag is set.
   */
  static bool IsRequested() noexcept;

  /** Publishes stop_requested to IsRequested() on this thread for the lifetime of the scope. */
  class Scope
  {
  public:
    explicit Scope(const std::atomic<bool> &stop_requested) noexcept;
    ~Scope() noexcept;

    Scope(const Scope &)            = delete;
    Scope(Scope &&)                 = delete;
    Scope &operator=(const Scope &) = delete;
    Scope &operator=(Scope &&)      = delete;

  private:
    const std::atomic<bool> *previous_;
  };
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
