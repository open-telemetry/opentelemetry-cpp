#pragma once

#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "vector"

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
  InMemorySpanData(size_t buffesr_size) : spans_received_(buffer_size) {}

  void Add(std::unique_ptr<opentelemetry::sdk::trace::Recordable> data) noexcept
  {
    std::unique_ptr<opentelemetry::sdk::trace::SpanData> span_data(
        static_cast<opentelemetry::sdk::trace::SpanData *>(data.release()));
    spans_received_.Add(span_data);
  }

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>> GetSpans() noexcept
  {
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>> res;

    spans_received_.Consume(
        spans_received_.size(), [&](opentelemetry::sdk::common::CircularBufferRange<
                                    opentelemetry::sdk::common::AtomicUniquePtr<
                                        opentelemetry::sdk::trace::SpanData>> range) noexcept {
          range.ForEach([&](
              opentelemetry::sdk::common::AtomicUniquePtr<opentelemetry::sdk::trace::SpanData> &
              ptr) noexcept {
            std::unique_ptr<opentelemetry::sdk::trace::SpanData> swap_ptr =
                std::unique_ptr<opentelemetry::sdk::trace::SpanData>(nullptr);
            ptr.Swap(swap_ptr);
            res.push_back(std::unique_ptr<opentelemetry::sdk::trace::SpanData>(swap_ptr.release()));
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
