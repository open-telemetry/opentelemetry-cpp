#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace mock
{
/**
 * A mock exporter that switches a flag once a valid recordable was received.
 */
class MockSpanExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  MockSpanExporter(std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received);

  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override;

  sdk::trace::ExportResult Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &recordables) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override {};

private:
  std::shared_ptr<std::vector<std::unique_ptr<sdk::trace::SpanData>>> spans_received_;
};
}  // namespace mock
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
