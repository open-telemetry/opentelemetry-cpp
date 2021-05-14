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

#include "opentelemetry/common/spin_lock_mutex.h"

#include <benchmark/benchmark.h>
#include <mutex>

namespace
{
using opentelemetry::common::SpinLockMutex;

constexpr int TightLoopLocks = 10000;

// Runs a thrash-test where we spin up N threads, each of which will
// attempt to lock-mutate-unlock a total of `TightLoopLocks` times.
//
// lock: A lambda denoting how to lock.   Accepts a reference to `SpinLockType`.
// unlock: A lambda denoting how to unlock.   Accepts a reference to `SpinLockType`.
template <typename SpinLockType, typename LockF, typename UnlockF>
inline void SpinThrash(benchmark::State &s, SpinLockType &spinlock, LockF lock, UnlockF unlock)
{
  auto num_threads = s.range(0);
  // Value we will increment, fighting over a spinlock.
  // The contention is meant to be brief, as close to our expected
  // use cases of "updating pointers" or "pushing an event onto a buffer".
  std::int64_t value = 0;

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
          lock(spinlock);
          value++;
          unlock(spinlock);
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
static void BM_SpinLockThrashing(benchmark::State &s)
{
  SpinLockMutex spinlock;
  SpinThrash(
      s, spinlock, [](SpinLockMutex &m) { m.lock(); }, [](SpinLockMutex &m) { m.unlock(); });
}

// Naive `while(try_lock()) {}` implementation of lock.
static void BM_NaiveSpinLockThrashing(benchmark::State &s)
{
  SpinLockMutex spinlock;
  SpinThrash(
      s, spinlock,
      [](SpinLockMutex &m) {
        while (!m.try_lock())
        {
        }
      },
      [](SpinLockMutex &m) { m.unlock(); });
}

// Simple `while(try_lock()) { yield-processor }`
static void BM_ProcYieldSpinLockThrashing(benchmark::State &s)
{
  SpinLockMutex spinlock;
  SpinThrash<SpinLockMutex>(
      s, spinlock,
      [](SpinLockMutex &m) {
        while (!m.try_lock())
        {
#if defined(_MSC_VER)
          YieldProcessor();
#elif defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__)
          _mm_pause();
#  else
          __builtin_ia32_pause();
#  endif
#elif defined(__arm__)
          __yield();
#endif
        }
      },
      [](SpinLockMutex &m) { m.unlock(); });
}

// SpinLock thrashing with thread::yield() after N spins.
static void BM_ThreadYieldSpinLockThrashing(benchmark::State &s)
{
  std::atomic<bool> mutex(false);
  SpinThrash<std::atomic<bool>>(
      s, mutex,
      [](std::atomic<bool> &l) {
        if (!l.exchange(true, std::memory_order_acquire))
        {
          return;
        }
        for (std::size_t i = 0; i < 100; ++i)
        {
          if (!l.load(std::memory_order_acquire) && !l.exchange(true, std::memory_order_acquire))
          {
            return;
          }
        }
        std::this_thread::yield();
      },
      [](std::atomic<bool> &l) { l.store(false, std::memory_order_release); });
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

}  // namespace

BENCHMARK_MAIN();
