// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "opentelemetry/nostd/span.h"
#include "opentelemetry/sdk/common/atomic_unique_ptr.h"
#include "opentelemetry/sdk/common/circular_buffer_range.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace common
{
/*
 * A lock-free circular buffer that supports multiple concurrent producers
 * and a single consumer.
 *
 * A producer claims a slot with a compare-and-swap on head_, which never
 * gets more than max_size() ahead of tail_, then stores its element into the
 * slot. The consumer advances tail_ only after it has emptied the consumed
 * slots, so a claimed slot is always empty.
 */
template <class T>
class CircularBuffer
{
public:
  explicit CircularBuffer(size_t max_size)
      : data_{new AtomicUniquePtr<T>[max_size + 1]}, capacity_{max_size + 1}
  {}

  /**
   * @return a range of the elements in the circular buffer
   *
   * Note: This method must only be called from the consumer thread.
   */
  CircularBufferRange<const AtomicUniquePtr<T>> Peek() const noexcept
  {
    return WaitForElements(PeekImpl());
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
    assert(n <= static_cast<size_t>(head_ - tail_));
    auto range = WaitForElements(PeekImpl().Take(n));
    static_assert(noexcept(callback(range)), "callback not allowed to throw");
    callback(range);
    // The callback has emptied the slots, so producers may now reuse them.
    tail_ += n;
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
    // A null element would leave a slot that the consumer waits on forever.
    if (ptr == nullptr)
    {
      return false;
    }

    // Claim a slot if the buffer is not full. A rejected Add changes nothing.
    uint64_t head = head_.load();
    for (;;)
    {
      const uint64_t tail = tail_.load();
      if (head < tail)
      {
        // The consumer has moved past our snapshot of head_.
        head = head_.load();
        continue;
      }
      if (head - tail >= capacity_ - 1)
      {
        return false;
      }
      if (head_.compare_exchange_strong(head, head + 1))
      {
        break;
      }
    }
    data_[head % capacity_].Swap(ptr);

    // ptr now holds the previous value of the slot, which is null as long as
    // Consume callbacks empty the slots they are given.
    ptr.reset();
    return true;
  }

  bool Add(std::unique_ptr<T> &&ptr) noexcept
  {
    std::unique_ptr<T> owner = std::move(ptr);
    return Add(owner);
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
  bool empty() const noexcept { return head_ == tail_; }

  /**
   * @return the number of bytes stored in the circular buffer.
   *
   * Note: this method will only return a correct snapshot of the size if called
   * from the consumer thread.
   */
  size_t size() const noexcept
  {
    uint64_t tail = tail_;
    uint64_t head = head_;
    assert(tail <= head);
    return head - tail;
  }

  /**
   * @return the number of elements consumed from the circular buffer.
   */
  uint64_t consumption_count() const noexcept { return tail_; }

  /**
   * @return the number of elements added, or being added, to the circular buffer.
   */
  uint64_t production_count() const noexcept { return head_; }

private:
  std::unique_ptr<AtomicUniquePtr<T>[]> data_;
  size_t capacity_;
  // Slots claimed by producers. A claimed slot may not be filled yet.
  std::atomic<uint64_t> head_{0};
  std::atomic<uint64_t> tail_{0};

  /**
   * A producer stores its element only after it has claimed a slot, so wait
   * for the producers that have claimed a slot in the range but not stored
   * into it yet. The rest of their Add() never waits or retries.
   */
  static CircularBufferRange<AtomicUniquePtr<T>> WaitForElements(
      const CircularBufferRange<AtomicUniquePtr<T>> &range) noexcept
  {
    range.ForEach([](const AtomicUniquePtr<T> &ptr) noexcept {
      while (ptr.IsNull())
      {
        std::this_thread::yield();
      }
      return true;
    });
    return range;
  }

  CircularBufferRange<AtomicUniquePtr<T>> PeekImpl() const noexcept
  {
    uint64_t tail_index = tail_ % capacity_;
    uint64_t head_index = head_ % capacity_;
    if (head_index == tail_index)
    {
      return {};
    }
    AtomicUniquePtr<T> *data = data_.get();
    if (tail_index < head_index)
    {
      return CircularBufferRange<AtomicUniquePtr<T>>{nostd::span<AtomicUniquePtr<T>>{
          data + tail_index, static_cast<std::size_t>(head_index - tail_index)}};
    }
    return {nostd::span<AtomicUniquePtr<T>>{data + tail_index,
                                            static_cast<std::size_t>(capacity_ - tail_index)},
            nostd::span<AtomicUniquePtr<T>>{data, static_cast<std::size_t>(head_index)}};
  }
};
}  // namespace common
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
