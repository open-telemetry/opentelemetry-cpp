// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
/**
 * A wrapper class holding in memory exporter data
 */
class InMemorySpanData final
{
public:
  /**
   * @param buffer_size a required value that sets the size of the CircularBuffer
   */
  InMemorySpanData(size_t buffer_size) : spans_received_(buffer_size) {}

  /**
   * @param data a required unique pointer to the data to add to the CircularBuffer
   */
  void Add(std::unique_ptr<opentelemetry::sdk::trace::SpanData> data) noexcept
  {
    std::unique_ptr<opentelemetry::sdk::trace::SpanData> span_data(
        static_cast<opentelemetry::sdk::trace::SpanData *>(data.release()));
    spans_received_.Add(span_data);
  }

  /**
   * @return Returns a vector of unique pointers containing all the span data in the
   * CircularBuffer. This operation will empty the Buffer, which is why the data
   * is returned as unique pointers
   */
  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>> GetSpans() noexcept
  {
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>> res;

    // Pointer swap is required because the Consume function requires that the
    // AtomicUniquePointer be set to null
    spans_received_.Consume(
        spans_received_.size(),
        [&](opentelemetry::sdk::common::CircularBufferRange<
            opentelemetry::sdk::common::AtomicUniquePtr<opentelemetry::sdk::trace::SpanData>>
                range) noexcept {
          range.ForEach(
              [&](opentelemetry::sdk::common::AtomicUniquePtr<opentelemetry::sdk::trace::SpanData>
                      &ptr) noexcept {
                std::unique_ptr<opentelemetry::sdk::trace::SpanData> swap_ptr =
                    std::unique_ptr<opentelemetry::sdk::trace::SpanData>(nullptr);
                ptr.Swap(swap_ptr);
                res.push_back(
                    std::unique_ptr<opentelemetry::sdk::trace::SpanData>(swap_ptr.release()));
                return true;
              });
        });

    return res;
  }

private:
  opentelemetry::sdk::common::CircularBuffer<opentelemetry::sdk::trace::SpanData> spans_received_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
