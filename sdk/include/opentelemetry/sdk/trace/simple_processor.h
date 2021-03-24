#pragma once

#include <atomic>
#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/exportable_span.h"

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
 *
 * All calls to the configured SpanExporter are synchronized using a
 * spin-lock on an atomic_flag.
 */
class SimpleSpanProcessor : public SpanProcessor
{
public:
  /**
   * Initialize a simple span processor.
   * @param exporter the exporter used by the span processor
   */
  explicit SimpleSpanProcessor(std::unique_ptr<SpanExporter> &&exporter) noexcept
      : exporter_(std::move(exporter))
  {}

  void RegisterRecordable(ExportableSpan& span) noexcept override
  {
    span.RegisterRecordableFor(*this, exporter_->MakeRecordable());
  }

  void OnStart(ExportableSpan &span,
               const opentelemetry::trace::SpanContext &parent_context) noexcept override
  {}

  void OnEnd(std::unique_ptr<ExportableSpan> &&span) noexcept override
  {
    auto data = span->ReleaseRecordableFor(*this);
    nostd::span<std::unique_ptr<Recordable>> batch(&data, 1);
    const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
    if (exporter_->Export(batch) == ExportResult::kFailure)
    {
      /* Once it is defined how the SDK does logging, an error should be
       * logged in this case. */
    }
  }

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override
  {
    return true;
  }

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override
  {
    // We only call shutdown ONCE.
    if (exporter_ != nullptr && !shutdown_latch_.test_and_set(std::memory_order_acquire))
    {
      return exporter_->Shutdown(timeout);
    }
    return true;
  }

  ~SimpleSpanProcessor() { Shutdown(); }

private:
  std::unique_ptr<SpanExporter> exporter_;
  opentelemetry::common::SpinLockMutex lock_;
  std::atomic_flag shutdown_latch_ = ATOMIC_FLAG_INIT;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
