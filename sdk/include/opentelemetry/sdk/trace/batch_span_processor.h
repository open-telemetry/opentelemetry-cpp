// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "opentelemetry/sdk/common/circular_buffer.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class SpanExporter;
struct BatchSpanProcessorOptions;

/**
 * This is an implementation of the SpanProcessor which creates batches of finished spans and passes
 * the export-friendly span data representations to the configured SpanExporter.
 */
class OPENTELEMETRY_EXPORT BatchSpanProcessor : public SpanProcessor
{
public:
  /**
   * Creates a batch span processor by configuring the specified exporter and other parameters
   * as per the official, language-agnostic opentelemetry specs.
   *
   * @param exporter - The backend exporter to pass the ended spans to.
   * @param options - The batch SpanProcessor options.
   */
  BatchSpanProcessor(std::unique_ptr<SpanExporter> &&exporter,
                     const BatchSpanProcessorOptions &options);

  /**
   * Requests a Recordable(Span) from the configured exporter.
   *
   * @return A recordable generated by the backend exporter
   */
  std::unique_ptr<Recordable> MakeRecordable() noexcept override;

  /**
   * Called when a span is started.
   *
   * NOTE: This method is a no-op.
   *
   * @param span - The span that just started
   * @param parent_context - The parent context of the span that just started
   */
  void OnStart(Recordable &span,
               const opentelemetry::trace::SpanContext &parent_context) noexcept override;

  /**
   * Called when a span ends.
   *
   * @param span - A recordable for a span that just ended
   */
  void OnEnd(std::unique_ptr<Recordable> &&span) noexcept override;

  /**
   * Export all ended spans that have not been exported yet.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  /**
   * Shuts down the processor and does any cleanup required. Completely drains the buffer/queue of
   * all its ended spans and passes them to the exporter. Any subsequent calls to OnStart, OnEnd,
   * ForceFlush or Shutdown will return immediately without doing anything.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  /**
   * Class destructor which invokes the Shutdown() method. The Shutdown() method is supposed to be
   * invoked when the Tracer is shutdown (as per other languages), but the C++ Tracer only takes
   * shared ownership of the processor, and thus doesn't call Shutdown (as the processor might be
   * shared with other Tracers).
   */
  ~BatchSpanProcessor() override;

protected:
  /**
   * The background routine performed by the worker thread.
   */
  void DoBackgroundWork();

  /**
   * Exports all ended spans to the configured exporter.
   *
   */
  virtual void Export();

  /**
   * Called when Shutdown() is invoked. Completely drains the queue of all its ended spans and
   * passes them to the exporter.
   */
  void DrainQueue();

  struct SynchronizationData
  {
    /* Synchronization primitives */
    std::condition_variable cv, force_flush_cv;
    std::mutex cv_m, force_flush_cv_m, shutdown_m;

    /* Important boolean flags to handle the workflow of the processor */
    std::atomic<bool> is_force_wakeup_background_worker{false};
    std::atomic<bool> is_force_flush_pending{false};
    std::atomic<bool> is_force_flush_notified{false};
    std::atomic<std::chrono::microseconds::rep> force_flush_timeout_us{0};
    std::atomic<bool> is_shutdown{false};
  };

  /**
   * @brief Notify completion of shutdown and force flush. This may be called from the any thread at
   * any time
   *
   * @param notify_force_flush Flag to indicate whether to notify force flush completion.
   * @param synchronization_data Synchronization data to be notified.
   */
  static void NotifyCompletion(bool notify_force_flush,
                               const std::unique_ptr<SpanExporter> &exporter,
                               const std::shared_ptr<SynchronizationData> &synchronization_data);

  void GetWaitAdjustedTime(std::chrono::microseconds &timeout,
                           std::chrono::time_point<std::chrono::system_clock> &start_time);
  /* The configured backend exporter */
  std::unique_ptr<SpanExporter> exporter_;

  /* Configurable parameters as per the official specs */
  const size_t max_queue_size_;
  const std::chrono::milliseconds schedule_delay_millis_;
  const size_t max_export_batch_size_;

  /* The buffer/queue to which the ended spans are added */
  opentelemetry::sdk::common::CircularBuffer<Recordable> buffer_;

  std::shared_ptr<SynchronizationData> synchronization_data_;

  /* The background worker thread */
  std::thread worker_thread_;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
