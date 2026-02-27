// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

/**
 * A multi-threaded stress test framework to measure throughput and performance of a given workload.
 *
 * ## Overview
 * Multi-threaded stress test framework designed to execute a specified function
 * in parallel across multiple threads and measure its throughput. The results are displayed
 * dynamically, including current throughput, average throughput, and minimum/maximum throughput
 * during the test.
 *
 * ## Key Features
 * - **Multi-threading**: Uses std::thread to execute the workload in parallel across a user-defined
 * number of threads.
 * - **Thread Safety**: Tracks iteration counts per thread using an aligned and padded structure
 * (WorkerStats) to avoid false sharing and ensure efficient thread-safe updates.
 * - **Dynamic Metrics**: Continuously calculates and displays throughput (iterations/sec) over
 * sliding time windows.
 * - **Graceful Termination**: Captures signals (e.g., Ctrl+C) to cleanly stop all threads and
 * summarize the results.
 * - **Thread Affinity (Linux-only)**: Optionally binds threads to specific CPU cores for consistent
 * performance.
 *
 * ## Implementation Details
 * - **Worker Threads**:
 *   - Each worker thread executes the workload function (func) in a loop until a global STOP flag
 * is set.
 *   - Each thread maintains its own iteration count to minimize contention.
 *
 * - **Throughput Monitoring**:
 *   - A separate controller thread monitors throughput by periodically summing up iteration counts
 * across threads.
 *   - Throughput is calculated over a sliding window (SLIDING_WINDOW_SIZE) and displayed
 * dynamically.
 *
 * - **Thread Synchronization**:
 *   - The STOP flag, an std::atomic<bool>, ensures all threads stop gracefully when signaled.
 *   - Memory ordering (e.g., std::memory_order_relaxed, std::memory_order_acquire/release) is used
 * to optimize performance while maintaining correctness.
 *
 * - **Final Summary**:
 *   - At the end of the test, the program calculates and prints the total iterations, duration, and
 * average throughput.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

// Configuration constants
constexpr uint64_t SLIDING_WINDOW_SIZE = 2;   // Time window for throughput calculation (in seconds)
constexpr size_t CACHE_LINE_SIZE       = 64;  // Typical CPU cache line size for alignment

// WorkerStats structure for tracking iteration counts per thread
struct alignas(CACHE_LINE_SIZE) WorkerStats
{
  std::atomic<uint64_t> count{0};  // Count of iterations for a specific thread
  char padding[CACHE_LINE_SIZE -
               sizeof(std::atomic<uint64_t>)];  // Padding to ensure proper alignment
};

// StressTest class
class Stress
{
public:
  // Constructor
  Stress(std::function<void()> func, size_t numThreads = std::thread::hardware_concurrency());

  // Main function to start the stress test
  void run();

  // function to stop the test
  void stop();

private:
  std::function<void()> func_;         // Function to be executed by each thread
  std::vector<std::thread> threads_;   // Vector to hold worker threads
  std::vector<WorkerStats> stats_;     // Vector to hold statistics for each thread
  const size_t numThreads_;            // Number of threads to run
  std::atomic<bool> stopFlag_{false};  // signal to stop the test

  // Worker thread function
  void workerThread(size_t threadIndex);

  // Monitoring thread function to calculate and display throughput
  void monitorThroughput();

  // Helper function to format numbers with commas for readability
  static std::string formatNumber(uint64_t num);
};
