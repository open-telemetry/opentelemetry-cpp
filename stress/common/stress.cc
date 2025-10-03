// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "stress.h"

// Global flags
std::atomic<bool> STOP(
    false);  // Global flag to stop the stress test when signaled (e.g., via Ctrl+C)
std::atomic<bool> READY(false);  // Global flag to synchronize thread start

// StressTest constructor
Stress::Stress(std::function<void()> func, size_t numThreads)
    : func_(std::move(func)), stats_(numThreads), numThreads_(numThreads)
{}

// Main function to start the stress test
void Stress::run()
{
  std::cout << "Starting stress test with " << numThreads_ << " threads...\n";
  auto startTime = std::chrono::steady_clock::now();

  READY.store(false, std::memory_order_release);

  std::thread controllerThread(&Stress::monitorThroughput, this);

  threads_.reserve(numThreads_);
  for (size_t i = 0; i < numThreads_; ++i)
  {
    threads_.emplace_back(&Stress::workerThread, this, i);
  }

  READY.store(true, std::memory_order_release);

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

  std::cout << "\nTest completed:\n"
            << "Total iterations: " << formatNumber(totalCount) << "\n"
            << "Duration: " << duration.count() << " seconds\n"
            << "Average throughput: " << formatNumber(totalCount / duration.count())
            << " iterations/sec\n";
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

  while (!STOP.load(std::memory_order_acquire))
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
  std::vector<uint64_t> throughputHistory;

  while (!STOP.load(std::memory_order_acquire))
  {
    std::this_thread::sleep_for(std::chrono::seconds(SLIDING_WINDOW_SIZE));

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();

    uint64_t totalCount = 0;
    for (const auto &stat : stats_)
    {
      totalCount += stat.count.load(std::memory_order_relaxed);
    }

    uint64_t currentCount = totalCount - lastTotalCount;
    lastTotalCount        = totalCount;
    lastTime              = currentTime;

    if (elapsed > 0)
    {
      uint64_t throughput = currentCount / elapsed;
      throughputHistory.push_back(throughput);

      double avg   = 0;
      uint64_t min = throughput;
      uint64_t max = throughput;

      for (uint64_t t : throughputHistory)
      {
        avg += t;
        min = std::min(min, t);
        max = std::max(max, t);
      }
      avg /= throughputHistory.size();

      std::cout << "\rThroughput: " << formatNumber(throughput)
                << " it/s | Avg: " << formatNumber(static_cast<uint64_t>(avg))
                << " | Min: " << formatNumber(min) << " | Max: " << formatNumber(max) << std::flush;
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
  STOP.store(true, std::memory_order_release);
}
