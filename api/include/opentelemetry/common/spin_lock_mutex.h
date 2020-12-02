#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

constexpr int SPINLOCK_FAST_ITERATIONS  = 100;
constexpr int SPINLOCK_YIELD_ITERATIONS = 10;
constexpr int SPINRLOCK_SLEEP_MS        = 1;

/**
 * A Mutex which uses atomic flags and spin-locks instead of halting threads.
 *
 * This mutex uses an incremental back-off strategy with the following phases:
 * 1. A tight spin-lock loop (pending: using hardware PAUSE/YIELD instructions)
 * 2. A loop where the current thread yields control after checking the lock.
 * 3. Issuing a thread-sleep call before starting back in phase 1.
 *
 * This is meant to give a good balance of perofrmance and CPU consumption in
 * practice.
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
   * Attempts to lock the mutex.  Return immediately with `true` (success) or `false` (failure).
   */
  bool try_lock() noexcept
  {
    return !flag_.load(std::memory_order_relaxed) &&
           !flag_.exchange(true, std::memory_order_acquire);
  }

  /**
   * Blocks until a lock can be obtained for the current thread.
   *
   * This mutex will spin the current CPU waiting for the lock to be available.  This can have
   * decent performance in scenarios where there is low lock contention and lock-holders acheive
   * their work quickly.  It degrades in scenarios where locked tasks take a long time.
   */
  void lock() noexcept
  {
    for (;;)
    {
      // Try once
      if (!flag_.exchange(true, std::memory_order_acquire))
      {
        return;
      }
      // Spin-Fast
      for (std::size_t i = 0; i < SPINLOCK_FAST_ITERATIONS; ++i)
      {
        if (try_lock())
        {
          return;
        }
        // TODO: Issue PAUSE/YIELD instruction to reduce contention.
        // e.g. __builtin_ia32_pause() / YieldProcessor() / _mm_pause();
      }
      // Spin-Yield
      for (std::size_t i = 0; i < SPINLOCK_YIELD_ITERATIONS; ++i)
      {
        if (try_lock())
        {
          return;
        }
        std::this_thread::yield();
      }
      // Sleep and then start the whole process again.
      std::this_thread::sleep_for(std::chrono::milliseconds(SPINRLOCK_SLEEP_MS));
    }
    return;
  }
  /** Releases the lock held by the execution agent. Throws no exceptions. */
  void unlock() noexcept { flag_.store(false, std::memory_order_release); }

private:
  std::atomic<bool> flag_{0};
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
