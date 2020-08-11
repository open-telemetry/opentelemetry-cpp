#pragma once
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/exporters/memory/in_memory_span_data.h"

const size_t MAX_BUFFER_SIZE = 100;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
/**
 * A in memory exporter that switches a flag once a valid recordable was received
 * and keeps track of all received spans in memory.
 */
class InMemorySpanExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  InMemorySpanExporter(size_t buffer_size = MAX_BUFFER_SIZE)
      : data_(new opentelemetry::exporter::memory::InMemorySpanData(buffer_size))
  {}

  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<sdk::trace::Recordable>(new sdk::trace::SpanData);
  }

  sdk::trace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override
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

  void Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override{};

  std::shared_ptr<opentelemetry::exporter::memory::InMemorySpanData> GetData() noexcept
  {
    return data_;
  }

private:
  std::shared_ptr<opentelemetry::exporter::memory::InMemorySpanData> data_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
