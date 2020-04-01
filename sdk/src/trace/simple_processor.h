#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * The simple span processor passes finished recordables to the configured
 * SpanExporter, as soon as they are finished.
 *
 * OnEnd, ForceFlush and Shutdown are no-ops.
 */
class SimpleSpanProcessor
{
public:
  /**
   * Initialize a simple span processor.
   * @param exporter the exporter used by the span processor
   */
  SimpleSpanProcessor(std::unique_ptr<SpanExporter> &&exporter) noexcept
      : exporter_(std::move(exporter))
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept { return exporter_->MakeRecordable(); }

  void OnStart(Recordable &span) noexcept {}

  void OnEnd(std::unique_ptr<Recordable> &&span) noexcept
  {
    std::shared_ptr<Recordable> recordable{std::move(span)};
    nostd::span<std::shared_ptr<Recordable>> s(&recordable, 1);
    if (exporter_->Export(s) == ExportResult::rFailure)
    {
      /* Once it is defined how the SDK does logging, an error should be
       * logged in this case. */
    }
  }

  void ForceFlush(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept {}

  void Shutdown() noexcept {}

private:
  std::unique_ptr<SpanExporter> exporter_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
