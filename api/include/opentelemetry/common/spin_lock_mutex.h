// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <chrono>
#include <thread>

#include "opentelemetry/version.h"

#if defined(_MSC_VER)
#  define _WINSOCKAPI_  // stops including winsock.h
#  include <windows.h>
#elif defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__)
#    include <emmintrin.h>
#  elif defined(__INTEL_COMPILER)
#    include <immintrin.h>
#  endif
#endif

// --- Parking strategy selection for the final lock() back-off tier ----------
// The historical final back-off was std::this_thread::sleep_for(1 ms). On
// Windows that 1 ms is rounded up to the system timer quantum (~15.6 ms by
// default), so a contended waiter stalls for a full quantum and inflates tail
// latency (high p99). Instead the waiter PARKS and is woken immediately on
// unlock(), independent of the timer resolution. The member type is unchanged
// (std::atomic<bool>), so the class layout / ABI is preserved.
//
// Exactly one strategy is selected per translation unit:
//   * OTEL_SPINLOCK_PARK_STD   - C++20 std::atomic<bool>::wait / notify_one
//   * OTEL_SPINLOCK_PARK_WIN32 - Win32 WaitOnAddress / WakeByAddressSingle
//                                (MSVC targeting Windows 8+)
//   * OTEL_SPINLOCK_PARK_SLEEP - portable sleep_for fallback (pre-C++20 non-MSVC)
//
// NOTE: the selection depends on the C++ standard in effect. As with the rest of
// this header, all translation units linked together must be compiled with the
// same C++ standard / STL, otherwise a parking lock() could be paired with a
// non-notifying unlock().
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
#  define OTEL_SPINLOCK_PARK_STD 1
#elif defined(_MSC_VER) && defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0602)
#  define OTEL_SPINLOCK_PARK_WIN32 1
#  pragma comment(lib, "Synchronization.lib")
#else
#  define OTEL_SPINLOCK_PARK_SLEEP 1
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{

constexpr int SPINLOCK_FAST_ITERATIONS = 100;
#if defined(OTEL_SPINLOCK_PARK_SLEEP)
constexpr int SPINLOCK_SLEEP_MS = 1;
#endif

/**
 * A Mutex which uses atomic flags and spin-locks instead of halting threads.
 *
 * This mutex uses an incremental back-off strategy with the following phases:
 * 1. A tight spin-lock loop (pending: using hardware PAUSE/YIELD instructions)
 * 2. A loop where the current thread yields control after checking the lock.
 * 3. Parking the thread until unlock() wakes it, before starting back in phase 1
 *    (the portable pre-C++20 non-MSVC fallback still uses a short sleep here).
 *
 * Phase 3 was historically std::this_thread::sleep_for(1 ms); on Windows that is
 * rounded up to the ~15.6 ms timer quantum and inflated tail latency, so the
 * contended waiter now parks and is woken immediately on release.
 *
 * This is meant to give a good balance of performance and CPU consumption in
 * practice.
 *
 * This class implements the `BasicLockable` specification:
 * https://en.cppreference.com/w/cpp/named_req/BasicLockable
 */
class SpinLockMutex
{
public:
  SpinLockMutex() noexcept {}
  ~SpinLockMutex() noexcept                       = default;
  SpinLockMutex(const SpinLockMutex &)            = delete;
  SpinLockMutex &operator=(const SpinLockMutex &) = delete;
  SpinLockMutex(SpinLockMutex &&)                 = delete;
  SpinLockMutex &operator=(SpinLockMutex &&)      = delete;

  static inline void fast_yield() noexcept
  {
// Issue a Pause/Yield instruction while spinning.
#if defined(_MSC_VER)
    YieldProcessor();
#elif defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__) || defined(__INTEL_COMPILER)
    _mm_pause();
#  else
    __builtin_ia32_pause();
#  endif
#elif defined(__armel__) || defined(__ARMEL__)
    asm volatile("nop" ::: "memory");
#elif defined(__arm__) || defined(__aarch64__)  // arm big endian / arm64
    __asm__ __volatile__("yield" ::: "memory");
#else
    // TODO: Issue PAGE/YIELD on other architectures.
#endif
  }

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
        fast_yield();
      }
      // Yield then try again (goal ~100ns)
      std::this_thread::yield();
      if (try_lock())
      {
        return;
      }
      // Final back-off: PARK until unlock() wakes us, instead of sleep_for(1 ms).
      // sleep_for(1 ms) is rounded up to the ~15.6 ms Windows timer quantum;
      // parking wakes immediately on release and does not depend on the timer.
#if defined(OTEL_SPINLOCK_PARK_STD)
      // C++20 standard, platform-agnostic park (futex / WaitOnAddress / ulock).
      // Returns when flag_ != true (an unlock that raced the park is not lost) or
      // on a spurious wake; the enclosing for(;;) re-checks and re-parks either way.
      flag_.wait(true, std::memory_order_relaxed);
#elif defined(OTEL_SPINLOCK_PARK_WIN32)
      // Pre-C++20 MSVC: park on the flag byte via WaitOnAddress; unlock() wakes it
      // with WakeByAddressSingle. WaitOnAddress returns immediately if the flag no
      // longer equals `compare` (locked), so a released lock is never lost.
#  if defined(__cpp_lib_atomic_is_always_lock_free)
      static_assert(std::atomic<bool>::is_always_lock_free,
                    "WaitOnAddress requires an address-free, lock-free atomic");
#  endif
      bool compare = true;  // single byte, matches sizeof(flag_)
      static_assert(sizeof(flag_) == sizeof(compare),
                    "WaitOnAddress reads the same width from the flag and the compare value");
      WaitOnAddress(reinterpret_cast<volatile VOID *>(&flag_), &compare, sizeof(compare), INFINITE);
#else
      // Portable fallback (pre-C++20, non-MSVC). On non-Windows, sleep_for(1 ms)
      // is ~1 ms (no 15.6 ms quantum) so the tail is small. NOTE: pre-C++20 MinGW
      // is Windows and lands here too, so it is NOT fixed -- build MinGW with
      // C++20 to park.
      std::this_thread::sleep_for(std::chrono::milliseconds(SPINLOCK_SLEEP_MS));
#endif
    }
    return;
  }
  /** Releases the lock held by the execution agent. Throws no exceptions. */
  void unlock() noexcept
  {
    flag_.store(false, std::memory_order_release);
    // Wake one parked waiter, if any. With no waiter-count field (ABI-frozen)
    // this is unconditional so a parked waiter is never lost; both wakes are
    // cheap when there is no waiter.
#if defined(OTEL_SPINLOCK_PARK_STD)
    flag_.notify_one();
#elif defined(OTEL_SPINLOCK_PARK_WIN32)
    WakeByAddressSingle(reinterpret_cast<PVOID>(&flag_));
#endif
  }

private:
  std::atomic<bool> flag_{false};
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE

// The parking-strategy selection macros gate the inline member bodies above;
// they are not part of the public surface, so undefine them here to avoid
// leaking object-like macros from this header into consumer translation units.
#if defined(OTEL_SPINLOCK_PARK_STD)
#  undef OTEL_SPINLOCK_PARK_STD
#endif
#if defined(OTEL_SPINLOCK_PARK_WIN32)
#  undef OTEL_SPINLOCK_PARK_WIN32
#endif
#if defined(OTEL_SPINLOCK_PARK_SLEEP)
#  undef OTEL_SPINLOCK_PARK_SLEEP
#endif
