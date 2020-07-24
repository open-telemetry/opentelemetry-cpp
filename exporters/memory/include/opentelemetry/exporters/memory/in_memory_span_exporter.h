#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{
/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class InMemorySpanExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  InMemorySpanExporter(
      std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received,
      std::shared_ptr<bool> shutdown_called = std::shared_ptr<bool>(new bool(false)));

  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override;

  sdk::trace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

private:
  std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received_;
  std::shared_ptr<bool> shutdown_called_;
};
}  // namespace memory
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
