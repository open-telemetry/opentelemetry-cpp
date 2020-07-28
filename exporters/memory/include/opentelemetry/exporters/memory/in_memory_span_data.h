#pragma once

#include "opentelemetry/sdk/trace/span_data.h"
#include "src/common/atomic_unique_ptr.h"
#include "src/common/circular_buffer.h"
#include "src/common/circular_buffer_range.h"

using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;
using opentelemetry::sdk::common::AtomicUniquePtr;

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
  void Add(std::unique_ptr<sdk::trace::SpanData> data) noexcept {}

  std::vector<sdk::trace::SpanData> GetSpans() noexcept {}

private:
  CircularBuffer<sdk::trace::SpanData> spans_received_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
