#include "opentelemetry/exporters/memory/in_memory_span_data.h"
#include "opentelemetry/sdk/common/atomic_unique_ptr.h"
#include "opentelemetry/sdk/common/circular_buffer_range.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <iostream>

using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;
using opentelemetry::sdk::trace::Recordable;
using opentelemetry::sdk::trace::SpanData;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
InMemorySpanData::InMemorySpanData(size_t buffer_size) : spans_received_(buffer_size) {}

void InMemorySpanData::Add(std::unique_ptr<Recordable> data) noexcept
{
  std::unique_ptr<SpanData> span_data(static_cast<SpanData *>(data.release()));
  spans_received_.Add(span_data);
}

std::vector<std::unique_ptr<SpanData>> InMemorySpanData::GetSpans() noexcept
{
  std::vector<std::unique_ptr<SpanData>> res;

  spans_received_.Consume(
      spans_received_.size(), [&](CircularBufferRange<AtomicUniquePtr<SpanData>> range) noexcept {
        range.ForEach([&](AtomicUniquePtr<SpanData> & ptr) noexcept {
          std::unique_ptr<SpanData> swap_ptr = std::unique_ptr<SpanData>(nullptr);
          ptr.Swap(swap_ptr);
          res.push_back(std::unique_ptr<SpanData>(swap_ptr.release()));
          return true;
        });
      });

  return res;
}
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
