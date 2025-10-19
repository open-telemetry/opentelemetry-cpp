// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#if defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__) || defined(__INTEL_COMPILER)
#    include <emmintrin.h>  // for _mm_pause
#  endif
#endif

#include "opentelemetry/common/macros.h"
#include "opentelemetry/common/spin_lock_mutex.h"

namespace
{
using opentelemetry::common::SpinLockMutex;

constexpr int TightLoopLocks = 10000;

// Runs a thrash-test where we spin up N threads, each of which will
// attempt to lock-mutate-unlock a total of `TightLoopLocks` times.
//
// lock: A lambda denoting how to lock.   Accepts a reference to `SpinLockType`.
// unlock: A lambda denoting how to unlock.   Accepts a reference to `SpinLockType`.
template <typename LockF, typename UnlockF>
void SpinThrash(benchmark::State &s, LockF lock, UnlockF unlock)
{
  auto num_threads = s.range(0);
  // Value we will increment, fighting over a spinlock.
  // The contention is meant to be brief, as close to our expected
  // use cases of "updating pointers" or "pushing an event onto a buffer".
  std::int64_t value OPENTELEMETRY_MAYBE_UNUSED = 0;

  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Timing loop
  for (auto _ : s)
  {
    for (auto i = 0; i < num_threads; i++)
    {
      threads.emplace_back([&] {
        // Increment value once each time the lock is acquired.  Spin a few times
        // to ensure maximum thread contention.
        for (int i = 0; i < TightLoopLocks; i++)
        {
          lock();
          value++;
          unlock();
        }
      });
    }
    // Join threads
    for (auto &thread : threads)
      thread.join();
    threads.clear();
  }
}

// Benchmark of full spin-lock implementation.
void BM_SpinLockThrashing(benchmark::State &s)
{
  SpinLockMutex spinlock;
  SpinThrash(s, [&] { spinlock.lock(); }, [&] { spinlock.unlock(); });
}

// Naive `while(try_lock()) {}` implementation of lock.
void BM_NaiveSpinLockThrashing(benchmark::State &s)
{
  SpinLockMutex spinlock;
  SpinThrash(
      s,
      [&] {
        while (!spinlock.try_lock())
        {
          // Left this comment to keep the same format on old and new versions of clang-format
        }
      },
      [&] { spinlock.unlock(); });
}

// Simple `while(try_lock()) { yield-processor }`
void BM_ProcYieldSpinLockThrashing(benchmark::State &s)
{
  SpinLockMutex spinlock;
  SpinThrash(
      s,
      [&] {
        while (!spinlock.try_lock())
        {
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
#endif
        }
      },
      [&] { spinlock.unlock(); });
}

// SpinLock thrashing with thread::yield().
void BM_ThreadYieldSpinLockThrashing(benchmark::State &s)
{
#if defined(__cpp_lib_atomic_value_initialization) && \
    __cpp_lib_atomic_value_initialization >= 201911L
  std::atomic_flag mutex{};
#else
  alignas(8) std::atomic_flag mutex = ATOMIC_FLAG_INIT;
#endif
  SpinThrash(
      s,
      [&]() {
        while (mutex.test_and_set(std::memory_order_acq_rel))
        {
          std::this_thread::yield();
        }
      },
      [&] { mutex.clear(std::memory_order_release); });
}

void BM_StdMutexCheck(benchmark::State &s)
{
  std::mutex mtx;
  SpinThrash(s, [&] { mtx.lock(); }, [&] { mtx.unlock(); });
}

// Run the benchmarks at 2x thread/core and measure the amount of time to thrash around.
BENCHMARK(BM_SpinLockThrashing)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ProcYieldSpinLockThrashing)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK(BM_NaiveSpinLockThrashing)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ThreadYieldSpinLockThrashing)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdMutexCheck)
    ->RangeMultiplier(2)
    ->Range(1, std::thread::hardware_concurrency())
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

}  // namespace

BENCHMARK_MAIN();
