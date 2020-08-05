#pragma once

#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "vector"

const size_t MAX_BUFFER_SIZE = 100;

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
  InMemorySpanData(size_t = MAX_BUFFER_SIZE);

  void Add(std::unique_ptr<opentelemetry::sdk::trace::Recordable> data) noexcept;

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> GetSpans() noexcept;

private:
  opentelemetry::sdk::common::CircularBuffer<opentelemetry::sdk::trace::Recordable> spans_received_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
