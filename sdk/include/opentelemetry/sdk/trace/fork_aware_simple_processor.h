#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/fork_aware_span_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * The simple span processor passes finished recordables to the configured
 * SpanExporter, as soon as they are finished.
 *
 * OnEnd and ForceFlush are no-ops.
 */
class ForkAwareSimpleSpanProcessor : public ForkAwareSpanProcessor
{
public:
  /**
   * Initialize a simple span processor.
   * @param exporter the exporter used by the span processor
   */
  explicit ForkAwareSimpleSpanProcessor(std::unique_ptr<SpanExporter> &&exporter) noexcept
      : exporter_(std::move(exporter))
  {}

  std::unique_ptr<Recordable> MakeRecordable() noexcept override
  {
    return exporter_->MakeRecordable();
  }

  void OnStart(Recordable &span) noexcept override {}

  void OnEnd(std::unique_ptr<Recordable> &&span) noexcept override
  {
    nostd::span<std::unique_ptr<Recordable>> batch(&span, 1);
    if (exporter_->Export(batch) == ExportResult::kFailure)
    {
      /* Once it is defined how the SDK does logging, an error should be
       * logged in this case. */
    }
  }

  void ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {}

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    exporter_->Shutdown(timeout);
  }

  // Note: All fork handlers are essentially no-ops for a simple processor. This is essentially
  //       because each span is exported individually on each call to OnEnd() method, and there is
  //       no temporary storage/buffer for spans.

private:
  std::unique_ptr<SpanExporter> exporter_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
