// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "opentelemetry/version.h"

#if defined(_MSC_VER)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#elif defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__)
#    include <emmintrin.h>
#  endif
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

constexpr int SPINLOCK_FAST_ITERATIONS = 100;
constexpr int SPINLOCK_SLEEP_MS        = 1;

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
   * decent performance in scenarios where there is low lock contention and lock-holders achieve
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
      // Spin-Fast (goal ~10ns)
      for (std::size_t i = 0; i < SPINLOCK_FAST_ITERATIONS; ++i)
      {
        if (try_lock())
        {
          return;
        }
// Issue a Pause/Yield instruction while spinning.
#if defined(_MSC_VER)
        YieldProcessor();
#elif defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__)
        _mm_pause();
#  else
        __builtin_ia32_pause();
#  endif
#elif defined(__arm__)
        // This intrinsic should fail to be found if YIELD is not supported on the current
        // processor.
        __yield();
#else
        // TODO: Issue PAGE/YIELD on other architectures.
#endif
      }
      // Yield then try again (goal ~100ns)
      std::this_thread::yield();
      if (try_lock())
      {
        return;
      }
      // Sleep and then start the whole process again. (goal ~1000ns)
      std::this_thread::sleep_for(std::chrono::milliseconds(SPINLOCK_SLEEP_MS));
    }
    return;
  }
  /** Releases the lock held by the execution agent. Throws no exceptions. */
  void unlock() noexcept { flag_.store(false, std::memory_order_release); }

private:
  std::atomic<bool> flag_{false};
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
