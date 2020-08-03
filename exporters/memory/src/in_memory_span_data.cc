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
void InMemorySpanData::Add(std::unique_ptr<Recordable> data) noexcept
{
	spans_received_.Add(data);
}

std::vector<std::unique_ptr<Recordable>> InMemorySpanData::GetSpans() noexcept
{
	std::vector<std::unique_ptr<Recordable>> res;

	spans_received_.Consume(
		spans_received_.size(), [&](CircularBufferRange<AtomicUniquePtr<Recordable>> & range) noexcept {
			range.ForEach([&](AtomicUniquePtr<Recordable> & ptr) noexcept {
				std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
				ptr.Swap(swap_ptr);

				res.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
				return true;
			});
		});

	return res;
}
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
