#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
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
      span_data_.Add(std::move(span));
    }
  }

  return sdk::trace::ExportResult::kSuccess;
}

std::vector<sdk::trace::SpanData> InMemorySpanExporter::GetData() noexcept
{
  return span_data_.GetSpans();
}
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
