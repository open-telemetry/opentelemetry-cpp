#pragma once

#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"

#include <atomic>
#include <condition_variable>
#include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace trace
{

/** Instantiation options. */
struct MultiSpanProcessorOptions {};

/**
 * This is an implementation of the SpanProcessor which aggregates across an in-order list of
 * other span processors.
 */
class MultiSpanProcessor : public SpanProcessor
{
public:
  /**
   * Creates a batch span processor by configuring the specified exporter and other parameters
   * as per the official, language-agnostic opentelemetry specs.
   *
   * @param exporter - The backend exporter to pass the ended spans to.
   * @param options - The batch SpanProcessor options.
   */
  MultiSpanProcessor(std::vector<std::unique_ptr<SpanProcessor>> processors,
                     const MultiSpanProcessorOptions &options = {});

  /**
   * Registeres any needed `Recordable`s for a given span.
   */
  void RegisterRecordable(ExportableSpan& span) noexcept override;

  /**
   * Called when a span is started.
   *
   * NOTE: This method is a no-op.
   *
   * @param span - The span that just started
   * @param parent_context - The parent context of the span that just started
   */
  void OnStart(ExportableSpan &span,
               const opentelemetry::trace::SpanContext &parent_context) noexcept override;

  /**
   * Called when a span ends.
   *
   * @param span - A recordable for a span that just ended
   */
  void OnEnd(std::unique_ptr<ExportableSpan> &&span) noexcept override;

  /**
   * Export all ended spans that have not been exported yet.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool ForceFlush(std::chrono::microseconds timeout) noexcept override;

  /**
   * Shuts down the processor and does any cleanup required. Completely drains the buffer/queue of
   * all its ended spans and passes them to the exporter. Any subsequent calls to OnStart, OnEnd,
   * ForceFlush or Shutdown will return immediately without doing anything.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool Shutdown(std::chrono::microseconds timeout) noexcept override;

  /**
   * Class destructor which invokes the Shutdown() method. The Shutdown() method is supposed to be
   * invoked when the Tracer is shutdown (as per other languages), but the C++ Tracer only takes
   * shared ownership of the processor, and thus doesn't call Shutdown (as the processor might be
   * shared with other Tracers).
   */
  ~MultiSpanProcessor();

private:
    std::vector<std::unique_ptr<SpanProcessor>> processors_;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
