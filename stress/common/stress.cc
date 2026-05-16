// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "stress.h"

#include <algorithm>
#include <limits>
#include <locale>
#include <utility>

#ifdef __linux__
#  include <pthread.h>
#  include <sched.h>
#endif

// StressTest constructor
Stress::Stress(std::function<void()> func, size_t numThreads)
    : func_(std::move(func)), stats_(numThreads), numThreads_(numThreads)
{}

// Main function to start the stress test
void Stress::run()
{
  std::cout << "Starting stress test with " << numThreads_ << " threads...\n";
  stopFlag_.store(false, std::memory_order_release);
  readyFlag_.store(false, std::memory_order_release);

  auto startTime = std::chrono::steady_clock::now();

  std::thread controllerThread(&Stress::monitorThroughput, this);

  threads_.reserve(numThreads_);
  for (size_t i = 0; i < numThreads_; ++i)
  {
    threads_.emplace_back(&Stress::workerThread, this, i);
  }

  readyFlag_.store(true, std::memory_order_release);

  for (auto &thread : threads_)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }

  if (controllerThread.joinable())
  {
    controllerThread.join();
  }

  auto endTime  = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

  uint64_t totalCount = 0;
  for (const auto &stat : stats_)
  {
    totalCount += stat.count.load(std::memory_order_relaxed);
  }

  const auto duration_seconds = duration.count();
  const auto average_throughput =
      duration_seconds > 0 ? totalCount / static_cast<uint64_t>(duration_seconds) : 0;

  std::cout << "\nTest completed:\n"
            << "Total iterations: " << formatNumber(totalCount) << "\n"
            << "Duration: " << duration_seconds << " seconds\n"
            << "Average throughput: " << formatNumber(average_throughput) << " iterations/sec\n";
}

// Worker thread function
void Stress::workerThread(size_t threadIndex)
{
#ifdef __linux__
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(threadIndex % std::thread::hardware_concurrency(), &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
#endif

  while (!readyFlag_.load(std::memory_order_acquire))
  {
    std::this_thread::yield();
  }

  while (!stopFlag_.load(std::memory_order_acquire))
  {
    func_();
    stats_[threadIndex].count.fetch_add(1, std::memory_order_relaxed);
  }
}

// Monitoring thread function
void Stress::monitorThroughput()
{
  uint64_t lastTotalCount = 0;
  auto lastTime           = std::chrono::steady_clock::now();
  uint64_t sampleCount    = 0;
  uint64_t throughputSum  = 0;
  uint64_t minThroughput  = std::numeric_limits<uint64_t>::max();
  uint64_t maxThroughput  = 0;

  while (!stopFlag_.load(std::memory_order_acquire))
  {
    std::this_thread::sleep_for(std::chrono::seconds(SLIDING_WINDOW_SIZE));

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime).count();

    uint64_t totalCount = 0;
    for (const auto &stat : stats_)
    {
      totalCount += stat.count.load(std::memory_order_relaxed);
    }

    uint64_t currentCount = totalCount - lastTotalCount;
    lastTotalCount        = totalCount;
    lastTime              = currentTime;

    if (elapsed > 0.0)
    {
      uint64_t throughput = static_cast<uint64_t>(static_cast<double>(currentCount) / elapsed);
      ++sampleCount;
      throughputSum += throughput;
      minThroughput                = std::min(minThroughput, throughput);
      maxThroughput                = std::max(maxThroughput, throughput);
      const uint64_t avgThroughput = throughputSum / sampleCount;

      std::cout << "\rThroughput: " << formatNumber(throughput)
                << " it/s | Avg: " << formatNumber(avgThroughput)
                << " | Min: " << formatNumber(minThroughput)
                << " | Max: " << formatNumber(maxThroughput) << std::flush;
    }
  }
  std::cout << std::endl;
}

// Helper function to format numbers with commas
std::string Stress::formatNumber(uint64_t num)
{
  std::ostringstream oss;
  oss.imbue(std::locale(""));
  oss << std::fixed << num;
  return oss.str();
}

// Signal handler to set the STOP flag when receiving a termination signal
void Stress::stop()
{
  stopFlag_.store(true, std::memory_order_release);
}
