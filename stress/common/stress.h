#ifndef STRESS_H
#define STRESS_H

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
  const size_t numThreads_;            // Number of threads to run
  std::function<void()> func_;         // Function to be executed by each thread
  std::vector<std::thread> threads_;   // Vector to hold worker threads
  std::vector<WorkerStats> stats_;     // Vector to hold statistics for each thread
  std::atomic<bool> stopFlag_{false};  // signal to stop the test

  // Worker thread function
  void workerThread(size_t threadIndex);

  // Monitoring thread function to calculate and display throughput
  void monitorThroughput();

  // Helper function to format numbers with commas for readability
  static std::string formatNumber(uint64_t num);
};

// Signal handler to set the STOP flag when receiving a termination signal
void signal_handler(int);

#endif  // STRESS_TEST_H
