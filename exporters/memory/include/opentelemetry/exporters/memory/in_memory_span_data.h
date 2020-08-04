#pragma once

#include "opentelemetry/sdk/trace/span_data.h"
#include "src/common/atomic_unique_ptr.h"
#include "src/common/circular_buffer.h"
#include "src/common/circular_buffer_range.h"
#include "opentelemetry/sdk/trace/recordable.h"

using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::trace::Recordable;

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
  InMemorySpanData();

  void Add(std::unique_ptr<Recordable> data) noexcept;

  std::vector<std::unique_ptr<Recordable>> GetSpans() noexcept;

private:
  CircularBuffer<Recordable> spans_received_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
