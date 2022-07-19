// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/common/circular_buffer.h"
#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/processor.h"

#  include <atomic>
#  include <condition_variable>
#  include <cstdint>
#  include <memory>
#  include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace logs
{

/**
 * Struct to hold batch SpanProcessor options.
 */
struct BatchLogProcessorOptions
{
  /**
   * The maximum buffer/queue size. After the size is reached, spans are
   * dropped.
   */
  size_t max_queue_size = 2048;

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(5000);

  /**
   * The maximum batch size of every export. It must be smaller or
   * equal to max_queue_size.
   */
  size_t max_export_batch_size = 512;
};

/**
 * This is an implementation of the LogProcessor which creates batches of finished logs and passes
 * the export-friendly log data representations to the configured LogExporter.
 */
class BatchLogProcessor : public LogProcessor
{
public:
  /**
   * Creates a batch log processor by configuring the specified exporter and other parameters
   * as per the official, language-agnostic opentelemetry specs.
   *
   * @param exporter - The backend exporter to pass the logs to
   * @param max_queue_size -  The maximum buffer/queue size. After the size is reached, logs are
   * dropped.
   * @param scheduled_delay_millis - The time interval between two consecutive exports.
   * @param max_export_batch_size - The maximum batch size of every export. It must be smaller or
   * equal to max_queue_size
   */
  explicit BatchLogProcessor(
      std::unique_ptr<LogExporter> &&exporter,
      const size_t max_queue_size                            = 2048,
      const std::chrono::milliseconds scheduled_delay_millis = std::chrono::milliseconds(5000),
      const size_t max_export_batch_size                     = 512);

  /**
   * Creates a batch log processor by configuring the specified exporter and other parameters
   * as per the official, language-agnostic opentelemetry specs.
   *
   * @param exporter - The backend exporter to pass the logs to
   * @param options - The batch SpanProcessor options.
   */
  explicit BatchLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                             const BatchLogProcessorOptions &options);

  /** Makes a new recordable **/
  std::unique_ptr<Recordable> MakeRecordable() noexcept override;

  /**
   * Called when the Logger's log method creates a log record
   * @param record the log record
   */

  void OnReceive(std::unique_ptr<Recordable> &&record) noexcept override;

  /**
   * Export all log records that have not been exported yet.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  /**
   * Shuts down the processor and does any cleanup required. Completely drains the buffer/queue of
   * all its logs and passes them to the exporter. Any subsequent calls to
   * ForceFlush or Shutdown will return immediately without doing anything.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  /**
   * Class destructor which invokes the Shutdown() method.
   */
  virtual ~BatchLogProcessor();

protected:
  /**
   * The background routine performed by the worker thread.
   */
  void DoBackgroundWork();

  /**
   * Exports all logs to the configured exporter.
   *
   */
  virtual void Export();

  /**
   * Called when Shutdown() is invoked. Completely drains the queue of all log records and
   * passes them to the exporter.
   */
  void DrainQueue();

  struct SynchronizationData
  {
    /* Synchronization primitives */
    std::condition_variable cv, force_flush_cv;
    std::mutex cv_m, force_flush_cv_m, shutdown_m;

    /* Important boolean flags to handle the workflow of the processor */
    std::atomic<bool> is_force_wakeup_background_worker;
    std::atomic<bool> is_force_flush_pending;
    std::atomic<bool> is_force_flush_notified;
    std::atomic<bool> is_shutdown;
  };

  /**
   * @brief Notify completion of shutdown and force flush. This may be called from the any thread at
   * any time
   *
   * @param notify_force_flush Flag to indicate whether to notify force flush completion.
   * @param synchronization_data Synchronization data to be notified.
   */
  static void NotifyCompletion(bool notify_force_flush,
                               const std::shared_ptr<SynchronizationData> &synchronization_data);

  void GetWaitAdjustedTime(std::chrono::microseconds &timeout,
                           std::chrono::time_point<std::chrono::system_clock> &start_time);

  /* The configured backend log exporter */
  std::unique_ptr<LogExporter> exporter_;

  /* Configurable parameters as per the official *trace* specs */
  const size_t max_queue_size_;
  const std::chrono::milliseconds scheduled_delay_millis_;
  const size_t max_export_batch_size_;
  /* The buffer/queue to which the ended logs are added */
  common::CircularBuffer<Recordable> buffer_;

  std::shared_ptr<SynchronizationData> synchronization_data_;

  /* The background worker thread */
  std::thread worker_thread_;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
