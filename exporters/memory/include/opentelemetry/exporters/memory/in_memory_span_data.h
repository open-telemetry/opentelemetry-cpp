#pragma once

#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/trace/recordable.h"
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
  InMemorySpanData();

  void Add(std::unique_ptr<opentelemetry::sdk::trace::Recordable> data) noexcept;

  std::vector<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> GetSpans() noexcept;

private:
  opentelemetry::sdk::common::CircularBuffer<opentelemetry::sdk::trace::Recordable> spans_received_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
