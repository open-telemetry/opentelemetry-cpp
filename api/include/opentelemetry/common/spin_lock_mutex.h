#pragma once

#include <atomic>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

/**
 * A Mutex which uses atomic flags and spin-locks instead of halting threads.
 *
 * This class implements the `BasicLockable` specification:
 * https://en.cppreference.com/w/cpp/named_req/BasicLockable
 */
class SpinLockMutex
{
public:
  SpinLockMutex() noexcept {}
  ~SpinLockMutex() noexcept            = default;
  SpinLockMutex(const SpinLockMutex &) = delete;
  SpinLockMutex &operator=(const SpinLockMutex &) = delete;
  SpinLockMutex &operator=(const SpinLockMutex &) volatile = delete;

  /**
   * Blocks until a lock can be obtained for the current thread.
   *
   * This mutex will spin the current CPU waiting for the lock to be available.  This can have
   * decent performance in scenarios where there is low lock contention and lock-holders acheive
   * their work quickly.  It degrades in scenarios where locked tasks take a long time.
   */
  void lock() noexcept
  {
    /* Note: We expect code protected by this lock to be "fast", i.e. we do NOT incrementally
     * back-off and wait/notify here, we just loop until we have access, then try again.
     *
     * This has the downside that we could be spinning a long time if the exporter is slow.
     * Note: in C++20x we could use `.wait` to make this slightly better. This should move to
     * an exponential-back-off / wait strategy.
     */
    while (flag_.test_and_set(std::memory_order_acquire))
      /** TODO - We should immmediately yield if the machine is single processor. */
      ;
  }
  /** Releases the lock held by the execution agent. Throws no exceptions. */
  void unlock() noexcept { flag_.clear(std::memory_order_release); }

private:
  std::atomic_flag flag_{ATOMIC_FLAG_INIT};
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
