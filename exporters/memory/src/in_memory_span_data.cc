#include "opentelemetry/exporters/memory/in_memory_span_data.h"

using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;
using opentelemetry::sdk::common::AtomicUniquePtr;

const size_t MAX_BUFFER_SIZE = 100;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
InMemorySpanData::InMemorySpanData()
{
  spans_received_ = std::move(new CircularBuffer<sdk::trace::SpanData>(MAX_BUFFER_SIZE));
}

void InMemorySpanData::Add(std::unique_ptr<sdk::trace::SpanData> data) noexcept
{
	spans_received_.Add(data);
}

std::vector<sdk::trace::SpanData> InMemorySpanData::GetSpans() noexcept
{
	std::vector<sdk::trace::SpanData> res;

	spans_received_.Consume(
		spans_received_.size(), [&](CircularBufferRange<AtomicUniquePtr<sdk::trace::SpanData>> & range) noexcept {
			range.ForEach([&](AtomicUniquePtr<sdk::trace::SpanData> & ptr) noexcept {
				res.push_back(*ptr);
				ptr.Reset();
				return true;
			});
		});

	return res;
}
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
