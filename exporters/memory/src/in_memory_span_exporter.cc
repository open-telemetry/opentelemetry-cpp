#include "opentelemetry/exporters/memory/in_memory_span_exporter.h"

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
InMemorySpanExporter::InMemorySpanExporter(
    std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received,
    std::shared_ptr<bool> shutdown_called)
    : spans_received_(spans_received), shutdown_called_(shutdown_called)
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
      spans_received_->push_back(std::move(span));
    }
  }

  return sdk::trace::ExportResult::kSuccess;
}

void InMemorySpanExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  *shutdown_called_ = true;
}
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
