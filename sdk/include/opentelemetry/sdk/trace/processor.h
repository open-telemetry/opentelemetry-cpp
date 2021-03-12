#pragma once

#include <chrono>
#include <memory>
#include "opentelemetry/sdk/trace/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

// Forward declaration to break circular dependency.
class Span;

/**
 * Span processor allow hooks for span start and end method invocations.
 *
 * Built-in span processors are responsible for batching and conversion of
 * spans to exportable representation and passing batches to exporters.
 */
class SpanProcessor
{
public:
  virtual ~SpanProcessor() = default;

  /**
   * Create a span recordable. This requests a new span recordable from the
   * associated exporter.
   * @return a newly initialized recordable
   *
   * Note: This method must be callable from multiple threads.
   */
  virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

  /**
   * OnStart is called when a span is started.
   * @param span a recordable for a span that was just started
   * @param parent_context The parent context of the span that just started
   */
  virtual void OnStart(Span &span,
                       const opentelemetry::trace::SpanContext &parent_context) noexcept = 0;

  /**
   * OnEnd is called when a span is ended.
   * @param span the span that ended.  Note: We need to pull our recordables off of this.
   */
  virtual void OnEnd(Span &span) noexcept = 0;

  /**
   * Export all ended spans that have not yet been exported.
   * @param timeout an optional timeout, the default timeout of 0 means that no
   * timeout is applied.
   */
  virtual bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept = 0;

  /**
   * Shut down the processor and do any cleanup required. Ended spans are
   * exported before shutdown. After the call to Shutdown, subsequent calls to
   * OnStart, OnEnd, ForceFlush or Shutdown will return immediately without
   * doing anything.
   * @param timeout an optional timeout, the default timeout of 0 means that no
   * timeout is applied.
   */
  virtual bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
