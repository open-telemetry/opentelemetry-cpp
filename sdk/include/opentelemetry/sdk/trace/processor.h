#pragma once

#include <chrono>
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
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

  // OnStart is called when a span is started.
  virtual void OnStart(opentelemetry::trace::Span &span) noexcept = 0;

  // OnEnd is called when a span is ended.
  virtual void OnEnd(
      std::shared_ptr<opentelemetry::sdk::trace::Recordable> &recordable) noexcept = 0;

  // Export all ended spans that have not yet been exported.
  //
  // Optionally a timeout can be specified. The default timeout of 0 means that
  // no timeout is applied.
  virtual void ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept = 0;

  // Shut down the processor and do any cleanup required.
  //
  // Ended spans are exported before shutdown. After the call to Shutdown,
  // subsequent calls to OnStart, OnEnd, ForceFlush or Shutdown will return
  // immediately without doing anything.
  virtual void Shutdown() noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
