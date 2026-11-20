// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "opentelemetry/sdk/common/atomic_unique_ptr.h"
#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/common/circular_buffer_range.h"

using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;

static std::mt19937 &RandomNumberGenerator()
{
  static thread_local std::mt19937 generator{std::random_device{}()};
  return generator;
}

static void GenerateRandomNumbers(CircularBuffer<uint32_t> &buffer,
                                  std::vector<uint32_t> &numbers,
                                  int n)
{
  for (int i = 0; i < n; ++i)
  {
    auto value = static_cast<uint32_t>(RandomNumberGenerator()());
    std::unique_ptr<uint32_t> x{new uint32_t{value}};
    if (buffer.Add(x))
    {
      numbers.push_back(value);
    }
  }
}

static void RunNumberProducers(CircularBuffer<uint32_t> &buffer,
                               std::vector<uint32_t> &numbers,
                               int num_threads,
                               int n)
{
  std::vector<std::vector<uint32_t>> thread_numbers(num_threads);
  std::vector<std::thread> threads(num_threads);
  for (int thread_index = 0; thread_index < num_threads; ++thread_index)
  {
    threads[thread_index] = std::thread{GenerateRandomNumbers, std::ref(buffer),
                                        std::ref(thread_numbers[thread_index]), n};
  }
  for (auto &thread : threads)
  {
    thread.join();
  }
  for (int thread_index = 0; thread_index < num_threads; ++thread_index)
  {
    numbers.insert(numbers.end(), thread_numbers[thread_index].begin(),
                   thread_numbers[thread_index].end());
  }
}

static void RunNumberConsumer(CircularBuffer<uint32_t> &buffer,
                              std::atomic<bool> &exit,
                              std::vector<uint32_t> &numbers)
{
  while (true)
  {
    if (exit && buffer.Peek().empty())
    {
      return;
    }
    auto n =
        std::uniform_int_distribution<size_t>{0, buffer.Peek().size()}(RandomNumberGenerator());
    buffer.Consume(n, [&](CircularBufferRange<AtomicUniquePtr<uint32_t>> range) noexcept {
      assert(range.size() == n);
      range.ForEach([&](AtomicUniquePtr<uint32_t> &ptr) noexcept {
        assert(!ptr.IsNull());
        numbers.push_back(*ptr);
        ptr.Reset();
        return true;
      });
    });
  }
}

TEST(CircularBufferTest, Add)
{
  CircularBuffer<int> buffer{10};

  std::unique_ptr<int> x{new int{11}};
  EXPECT_TRUE(buffer.Add(x));
  EXPECT_EQ(x, nullptr);
  auto range = buffer.Peek();
  EXPECT_EQ(range.size(), 1);
  range.ForEach([](const AtomicUniquePtr<int> &y) {
    EXPECT_EQ(*y, 11);
    return true;
  });
}

TEST(CircularBufferTest, Clear)
{
  CircularBuffer<int> buffer{10};

  std::unique_ptr<int> x{new int{11}};
  EXPECT_TRUE(buffer.Add(x));
  EXPECT_EQ(x, nullptr);
  buffer.Clear();
  EXPECT_TRUE(buffer.empty());
}

TEST(CircularBufferTest, AddOnFull)
{
  CircularBuffer<int> buffer{10};
  for (int i = 0; i < static_cast<int>(buffer.max_size()); ++i)
  {
    std::unique_ptr<int> x{new int{i}};
    EXPECT_TRUE(buffer.Add(x));
  }
  std::unique_ptr<int> x{new int{33}};
  EXPECT_FALSE(buffer.Add(x));
  EXPECT_NE(x, nullptr);
  EXPECT_EQ(*x, 33);
}

TEST(CircularBufferTest, Consume)
{
  CircularBuffer<int> buffer{10};
  for (int i = 0; i < static_cast<int>(buffer.max_size()); ++i)
  {
    std::unique_ptr<int> x{new int{i}};
    EXPECT_TRUE(buffer.Add(x));
  }
  int count = 0;
  buffer.Consume(5, [&](CircularBufferRange<AtomicUniquePtr<int>> range) noexcept {
    range.ForEach([&](AtomicUniquePtr<int> &ptr) {
      EXPECT_EQ(*ptr, count++);
      ptr.Reset();
      return true;
    });
  });
  EXPECT_EQ(count, 5);
}

TEST(CircularBufferTest, Simulation)
{
  const int num_producer_threads = 4;
  const int n                    = 25000;
  for (size_t max_size : {1, 2, 10, 50, 100, 1000})
  {
    CircularBuffer<uint32_t> buffer{max_size};
    std::vector<uint32_t> producer_numbers;
    std::vector<uint32_t> consumer_numbers;
    auto producers = std::thread{RunNumberProducers, std::ref(buffer), std::ref(producer_numbers),
                                 num_producer_threads, n};
    std::atomic<bool> exit{false};
    auto consumer = std::thread{RunNumberConsumer, std::ref(buffer), std::ref(exit),
                                std::ref(consumer_numbers)};
    producers.join();
    exit = true;
    consumer.join();
    std::sort(producer_numbers.begin(), producer_numbers.end());
    std::sort(consumer_numbers.begin(), consumer_numbers.end());

    EXPECT_EQ(producer_numbers.size(), consumer_numbers.size());
    EXPECT_EQ(producer_numbers, consumer_numbers);
  }
}

TEST(CircularBufferTest, WrapAround)
{
  CircularBuffer<uint64_t> buffer{7};
  uint64_t next_added    = 0;
  uint64_t next_consumed = 0;
  for (int lap = 0; lap < 100; ++lap)
  {
    // Add 1 to 7 elements; the ones that do not fit must be rejected.
    const size_t space = buffer.max_size() - buffer.size();
    for (size_t i = 0; i <= static_cast<size_t>(lap % 7); ++i)
    {
      std::unique_ptr<uint64_t> x{new uint64_t{next_added}};
      const bool added = buffer.Add(x);
      EXPECT_EQ(added, i < space);
      if (added)
      {
        ++next_added;
      }
      else
      {
        EXPECT_NE(x, nullptr);
      }
    }
    EXPECT_EQ(buffer.size(), next_added - next_consumed);
    EXPECT_EQ(buffer.production_count(), next_added);

    // Leave one element behind on odd laps so the tail moves around the buffer.
    const size_t num_consumed = buffer.size() - static_cast<size_t>(lap % 2);
    buffer.Consume(num_consumed,
                   [&](CircularBufferRange<AtomicUniquePtr<uint64_t>> range) noexcept {
                     EXPECT_EQ(range.size(), num_consumed);
                     range.ForEach([&](AtomicUniquePtr<uint64_t> &ptr) noexcept {
                       EXPECT_EQ(*ptr, next_consumed);
                       ++next_consumed;
                       ptr.Reset();
                       return true;
                     });
                   });
    EXPECT_EQ(buffer.consumption_count(), next_consumed);
  }
  buffer.Clear();
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.consumption_count(), buffer.production_count());
}

TEST(CircularBufferTest, AddOnFullConcurrent)
{
  const int num_threads = 16;
  const int n           = 1000;
  CircularBuffer<int> buffer{100};
  std::atomic<int> num_added{0};
  std::atomic<int> num_rejected{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int thread_index = 0; thread_index < num_threads; ++thread_index)
  {
    threads.emplace_back([&, thread_index] {
      for (int i = 0; i < n; ++i)
      {
        const int value = thread_index * n + i;
        std::unique_ptr<int> x{new int{value}};
        if (buffer.Add(x))
        {
          ++num_added;
        }
        else
        {
          // A rejected element is left with the caller.
          EXPECT_NE(x, nullptr);
          EXPECT_EQ(*x, value);
          ++num_rejected;
        }
      }
    });
  }
  for (auto &thread : threads)
  {
    thread.join();
  }
  EXPECT_EQ(num_added, 100);
  EXPECT_EQ(num_rejected, num_threads * n - 100);
  EXPECT_EQ(buffer.size(), 100);
  int num_consumed = 0;
  buffer.Consume(buffer.size(), [&](CircularBufferRange<AtomicUniquePtr<int>> range) noexcept {
    range.ForEach([&](AtomicUniquePtr<int> &ptr) noexcept {
      EXPECT_FALSE(ptr.IsNull());
      ++num_consumed;
      ptr.Reset();
      return true;
    });
  });
  EXPECT_EQ(num_consumed, 100);
  EXPECT_TRUE(buffer.empty());
}

TEST(CircularBufferTest, ConcurrentProducersKeepOrder)
{
  const uint64_t num_threads = 8;
  const uint64_t n           = 20000;
  for (size_t max_size : {1, 2, 3, 64})
  {
    CircularBuffer<uint64_t> buffer{max_size};
    std::vector<std::thread> producers;
    producers.reserve(num_threads);
    for (uint64_t thread_index = 0; thread_index < num_threads; ++thread_index)
    {
      producers.emplace_back([&, thread_index] {
        for (uint64_t i = 0; i < n; ++i)
        {
          std::unique_ptr<uint64_t> x{new uint64_t{(thread_index << 32) | i}};
          while (!buffer.Add(x))
          {
            std::this_thread::yield();
          }
        }
      });
    }

    // Each producer's elements must come out in the order it added them, and
    // Consume(size()) must hand out exactly size() elements even while
    // producers are in the middle of Add().
    std::vector<uint64_t> next(num_threads, 0);
    uint64_t num_consumed = 0;
    while (num_consumed < num_threads * n)
    {
      const size_t num_to_consume = buffer.size();
      if (num_to_consume == 0)
      {
        std::this_thread::yield();
        continue;
      }
      buffer.Consume(num_to_consume,
                     [&](CircularBufferRange<AtomicUniquePtr<uint64_t>> range) noexcept {
                       EXPECT_EQ(range.size(), num_to_consume);
                       range.ForEach([&](AtomicUniquePtr<uint64_t> &ptr) noexcept {
                         EXPECT_FALSE(ptr.IsNull());
                         const uint64_t thread_index = *ptr >> 32;
                         EXPECT_LT(thread_index, num_threads);
                         EXPECT_EQ(*ptr & 0xffffffff, next[thread_index]);
                         ++next[thread_index];
                         ++num_consumed;
                         ptr.Reset();
                         return true;
                       });
                     });
    }
    for (auto &producer : producers)
    {
      producer.join();
    }
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(num_consumed, num_threads * n);
    for (uint64_t thread_index = 0; thread_index < num_threads; ++thread_index)
    {
      EXPECT_EQ(next[thread_index], n);
    }
  }
}
