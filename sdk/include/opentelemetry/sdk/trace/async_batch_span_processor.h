// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_ASYNC_EXPORT

#  include "opentelemetry/sdk/common/circular_buffer.h"
#  include "opentelemetry/sdk/trace/batch_span_processor.h"
#  include "opentelemetry/sdk/trace/exporter.h"

#  include <atomic>
#  include <condition_variable>
#  include <queue>
#  include <thread>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace trace
{

/**
 * Struct to hold batch SpanProcessor options.
 */
struct AsyncBatchSpanProcessorOptions: public BatchSpanProcessorOptions
{
  /* Denotes the maximum number of async exports to continue
   */
  size_t max_export_async = 8;
};

/**
 * This is an implementation of the SpanProcessor which creates batches of finished spans and passes
 * the export-friendly span data representations to the configured SpanExporter.
 */
class AsyncBatchSpanProcessor : public BatchSpanProcessor
{
public:
  /**
   * Creates a batch span processor by configuring the specified exporter and other parameters
   * as per the official, language-agnostic opentelemetry specs.
   *
   * @param exporter - The backend exporter to pass the ended spans to.
   * @param options - The batch SpanProcessor options.
   */
  AsyncBatchSpanProcessor(std::unique_ptr<SpanExporter> &&exporter,
                          const AsyncBatchSpanProcessorOptions &options);

  /**
   * Shuts down the processor and does any cleanup required. Completely drains the buffer/queue of
   * all its ended spans and passes them to the exporter. Any subsequent calls to OnStart, OnEnd,
   * ForceFlush or Shutdown will return immediately without doing anything.
   *
   * NOTE: Timeout functionality not supported yet.
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  /**
   * Class destructor which invokes the Shutdown() method. The Shutdown() method is supposed to be
   * invoked when the Tracer is shutdown (as per other languages), but the C++ Tracer only takes
   * shared ownership of the processor, and thus doesn't call Shutdown (as the processor might be
   * shared with other Tracers).
   */
  virtual ~AsyncBatchSpanProcessor();

private:
  /**
   * Exports all ended spans to the configured exporter.
   *
   */
  void Export() override;

  struct ExportDataStorage
  {
    std::queue<size_t> export_ids;
    std::vector<bool> export_ids_flag;

    std::condition_variable async_export_waker;
    std::mutex async_export_data_m;
  };
  std::shared_ptr<ExportDataStorage> export_data_storage_;

  const size_t max_export_async_;
  static constexpr size_t kInvalidExportId = static_cast<size_t>(-1);

  /**
   * @brief Notify completion of shutdown and force flush. This may be called from the any thread at
   * any time
   *
   * @param notify_force_flush Flag to indicate whether to notify force flush completion.
   * @param synchronization_data Synchronization data to be notified.
   */
  static void NotifyCompletion(bool notify_force_flush,
                               const std::shared_ptr<SynchronizationData> &synchronization_data,
                               const std::shared_ptr<ExportDataStorage> &export_data_storage);
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif