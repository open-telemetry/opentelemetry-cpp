#pragma once

#include <chrono>
#include <memory>
#include <unordered_set>
#include <vector>
#include <utility>
#include <thread>

#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{
/*
 * The span processor passes and stores running and completed recordables (casted as span_data)
 * to be used by the TraceZ Data Aggregator.
 */
class TracezSpanProcessor : public opentelemetry::sdk::trace::SpanProcessor {
 public:

  struct CollectedSpans {
    std::unordered_set<opentelemetry::sdk::trace::SpanData*> running;
    std::vector<std::unique_ptr<opentelemetry::sdk::trace::SpanData>> completed;
  };

  /*
   * Initialize a span processor.
   */
  explicit TracezSpanProcessor() noexcept {}

  /*
   * Create a span recordable, which is span_data
   * @return a newly initialized recordable
   */
  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override
  {
    return std::unique_ptr<opentelemetry::sdk::trace::Recordable>(new opentelemetry::sdk::trace::SpanData);
  }

  /*
   * OnStart is called when a span starts; the recordable is cast to span_data and added to running_spans.
   * @param span a recordable for a span that was just started
   */
  void OnStart(opentelemetry::sdk::trace::Recordable &span) noexcept override;

  /*
   * OnEnd is called when a span ends; that span_data is moved from running_spans to
   * completed_spans
   * @param span a recordable for a span that was ended
   */
  void OnEnd(std::unique_ptr<opentelemetry::sdk::trace::Recordable> &&span) noexcept override;

  /*
   * Returns a snapshot of all spans stored. This snapshot has a copy of the
   * stored running_spans and gives ownership of completed spans to the caller.
   * Stored completed_spans are cleared from the processor. Currently,
   * copy-on-write is utilized where possible to minimize contention, but locks
   * may be added in the future.
   * @return snapshot of all currently running spans and newly completed spans
   * (spans never sent while complete) at the time that the function is called
   */
  CollectedSpans GetSpanSnapshot() noexcept;

  /*
   * For now, does nothing but sleep for the specified time. In the future, it
   * may send all ended spans that have not yet been sent to the aggregator.
   * @param timeout an optional timeout, the default timeout of 0 means that no
   * timeout is applied.
   */
  void ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    if (shutdown_signal_received_) return;
    std::this_thread::sleep_for(timeout);
  }

  /*
   * Shut down the processor and do any cleanup required through ForceFlush.
   * After the call to Shutdown, subsequent calls to OnStart, OnEnd, ForceFlush
   * or Shutdown will return immediately without doing anything.
   * @param timeout an optional timeout, the default timeout of 0 means that no
   * timeout is applied.
   */
  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
    ForceFlush(timeout);
    shutdown_signal_received_ = true;
  }

 private:
  CollectedSpans spans_;
  bool shutdown_signal_received_ = false;
};
}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
