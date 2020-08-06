#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"

using opentelemetry::exporter::memory::InMemorySpanData;
using opentelemetry::sdk::trace::Recordable;

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
InMemorySpanExporter::InMemorySpanExporter(size_t buffer_size)
    : data_(new InMemorySpanData(buffer_size))
{}

std::unique_ptr<sdk::trace::Recordable> InMemorySpanExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new sdk::trace::SpanData);
}

sdk::trace::ExportResult InMemorySpanExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept
{
  for (auto &recordable : recordables)
  {
    auto span = std::unique_ptr<sdk::trace::SpanData>(
        static_cast<sdk::trace::SpanData *>(recordable.release()));
    if (span != nullptr)
    {
      data_.get()->Add(std::move(span));
    }
  }

  return sdk::trace::ExportResult::kSuccess;
}

std::shared_ptr<opentelemetry::exporter::memory::InMemorySpanData>
InMemorySpanExporter::GetData() noexcept
{
  return data_;
}
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
