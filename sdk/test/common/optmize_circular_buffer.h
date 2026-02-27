// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/atomic_unique_ptr.h"
#include "opentelemetry/sdk/common/circular_buffer_range.h"
#include "opentelemetry/version.h"
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/*
 * An optimized lock-free circular buffer that supports multiple concurrent producers
 * and a single consumer. Uses FAA (Fetch-And-Add) for better performance.
 */
template <class T>
class OptimizedCircularBuffer
{
public:
  explicit OptimizedCircularBuffer(size_t max_size)
      : data_{new AtomicUniquePtr<T>[max_size + 1]}, capacity_{max_size + 1}
  {}

  /**
   * @return a range of the elements in the circular buffer
   *
   * Note: This method must only be called from the consumer thread.
   */
  CircularBufferRange<const AtomicUniquePtr<T>> Peek() const noexcept
  {
    return const_cast<OptimizedCircularBuffer *>(this)->PeekImpl();
  }

  /**
   * Consume elements from the circular buffer's tail.
   * @param n the number of elements to consume
   * @param callback the callback to invoke with an AtomicUniquePtr to each
   * consumed element.
   *
   * Note: The callback must set the passed AtomicUniquePtr to null.
   *
   * Note: This method must only be called from the consumer thread.
   */
  template <class Callback>
  void Consume(size_t n, Callback callback) noexcept
  {
    // comsume max(n, available) elements
    auto range = PeekImpl().Take(n);
    static_assert(noexcept(callback(range)), "callback not allowed to throw");
    // consume elements
    callback(range);
    // free elements to let producers add new elements
    tail_ += range.size();
    tail_ %= capacity_;
    count_.fetch_sub(range.size(), std::memory_order_release);
  }

  /**
   * Consume elements from the circular buffer's tail.
   * @param n the number of elements to consume
   *
   * Note: This method must only be called from the consumer thread.
   */
  void Consume(size_t n) noexcept
  {
    Consume(n, [](CircularBufferRange<AtomicUniquePtr<T>> &range) noexcept {
      range.ForEach([](AtomicUniquePtr<T> &ptr) noexcept {
        ptr.Reset();
        return true;
      });
    });
  }

  /**
   * Adds an element into the circular buffer.
   * @param ptr a pointer to the element to add
   * @return true if the element was successfully added; false, otherwise.
   */
  bool Add(std::unique_ptr<T> &ptr) noexcept
  {
    uint64_t count = count_.fetch_add(1, std::memory_order_acquire);
    if (count >= capacity_ - 1)
    {
      // queue is full, rollback
      count_.fetch_sub(1, std::memory_order_release);
      return false;
    }

    uint64_t head_pos = head_.fetch_add(1, std::memory_order_acquire);
    uint64_t head_index = head_pos % capacity_;
    // It should be a valid pos to add an element
    assert(data_[head_index].Get() == nullptr);
    
    // set the element must be sucess
    bool success = data_[head_index].SwapIfNull(ptr);
    assert(success); 
    ptr.reset();
    return true;
  }

  bool Add(std::unique_ptr<T> &&ptr) noexcept
  {
    // rvalue to lvalue reference
    bool result = Add(std::ref(ptr));
    ptr.reset();
    return result;
  }

  /**
   * Clear the circular buffer.
   *
   * Note: This method must only be called from the consumer thread.
   */
  void Clear() noexcept { Consume(size()); }

  /**
   * @return the maximum number of bytes that can be stored in the buffer.
   */
  size_t max_size() const noexcept { return capacity_ - 1; }

  /**
   * @return true if the buffer is empty.
   */
  bool empty() const noexcept { return count_.load(std::memory_order_relaxed) == 0; }

  /**
   * @return the number of bytes stored in the circular buffer.
   *
   * Note: this method will only return a correct snapshot of the size if called
   * from the consumer thread.
   */
  size_t size() const noexcept
  {
    return count_.load(std::memory_order_relaxed);
  }

  /**
   * @return the number of elements consumed from the circular buffer.
   */
  uint64_t consumption_count() const noexcept { return tail_; }

  /**
   * @return the number of elements added to the circular buffer.
   */
  uint64_t production_count() const noexcept { return head_; }

private:
  std::unique_ptr<AtomicUniquePtr<T>[]> data_;
  size_t capacity_;
  std::atomic<uint64_t> count_{0};  
  std::atomic<uint64_t> head_{0}; 
  uint64_t tail_{0};

  CircularBufferRange<AtomicUniquePtr<T>> PeekImpl() noexcept
  {
    uint64_t current_count = count_.load(std::memory_order_relaxed);
    if (current_count == 0)
    {
      return {};
    }
    
    auto data = data_.get();
    uint64_t available_count = 0;
    uint64_t max_check = std::min(current_count, capacity_);
    
    for (uint64_t i = 0; i < max_check; ++i)
    {
      uint64_t index = (tail_ + i) % capacity_;
      if (data[index].Get() != nullptr)
      {
        available_count++;
      }
      else
      {
        // Find the first null pointer, it's a element currently being added by producer 
        break;
      }
    }
    
    if (available_count == 0)
    {
      return {};
    }

    if (tail_ + available_count <= capacity_)
    {
      return CircularBufferRange<AtomicUniquePtr<T>>{nostd::span<AtomicUniquePtr<T>>{
          data + tail_, static_cast<std::size_t>(available_count)}};
    } else {
      // the elements are split into two parts
      uint64_t first_part_size = capacity_ - tail_;
      uint64_t second_part_size = available_count - first_part_size;
      
      return {nostd::span<AtomicUniquePtr<T>>{data + tail_, static_cast<std::size_t>(first_part_size)},
              nostd::span<AtomicUniquePtr<T>>{data, static_cast<std::size_t>(second_part_size)}};
    }
  }
};
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
