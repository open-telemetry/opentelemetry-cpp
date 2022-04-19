// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#ifdef ENABLE_ASYNC_EXPORT

#  include "opentelemetry/sdk/trace/async_batch_span_processor.h"
#  include "opentelemetry/common/spin_lock_mutex.h"

#  include <vector>
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;
using opentelemetry::trace::SpanContext;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

AsyncBatchSpanProcessor::AsyncBatchSpanProcessor(std::unique_ptr<SpanExporter> &&exporter,
                                                 const AsyncBatchSpanProcessorOptions &options)
    : BatchSpanProcessor(std::move(exporter), options.options),
      max_export_async_(options.max_export_async),
      export_data_storage_(std::make_shared<ExportDataStorage>())
{
  export_data_storage_->export_ids_flag.resize(max_export_async_, true);
  for (size_t i = 1; i <= max_export_async_; i++)
  {
    export_data_storage_->export_ids.push(i);
  }
}

void AsyncBatchSpanProcessor::Export()
{
  do
  {
    std::vector<std::unique_ptr<Recordable>> spans_arr;
    size_t num_records_to_export;
    bool notify_force_flush =
        synchronization_data_->is_force_flush_pending.exchange(false, std::memory_order_acq_rel);
    if (notify_force_flush)
    {
      num_records_to_export = buffer_.size();
    }
    else
    {
      num_records_to_export =
          buffer_.size() >= max_export_batch_size_ ? max_export_batch_size_ : buffer_.size();
    }

    if (num_records_to_export == 0)
    {
      NotifyCompletion(notify_force_flush, synchronization_data_, export_data_storage_);
      break;
    }
    buffer_.Consume(num_records_to_export,
                    [&](CircularBufferRange<AtomicUniquePtr<Recordable>> range) noexcept {
                      range.ForEach([&](AtomicUniquePtr<Recordable> &ptr) {
                        std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
                        ptr.Swap(swap_ptr);
                        spans_arr.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
                        return true;
                      });
                    });

    size_t id = kInvalidExportId;
    {
      std::unique_lock<std::mutex> lock(export_data_storage_->async_export_data_m);
      export_data_storage_->async_export_waker.wait_for(lock, schedule_delay_millis_, [this] {
        return export_data_storage_->export_ids.size() > 0;
      });
      if (export_data_storage_->export_ids.size() > 0)
      {
        id = export_data_storage_->export_ids.front();
        export_data_storage_->export_ids.pop();
        export_data_storage_->export_ids_flag[id - 1] = false;
      }
    }
    if (id != kInvalidExportId)
    {
      std::weak_ptr<ExportDataStorage> export_data_watcher = export_data_storage_;

      std::weak_ptr<SynchronizationData> synchronization_data_watcher = synchronization_data_;
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()),
          [notify_force_flush, synchronization_data_watcher, export_data_watcher,
           id](sdk::common::ExportResult result) {
            // TODO: Print result
            if (synchronization_data_watcher.expired())
            {
              return true;
            }

            if (export_data_watcher.expired())
            {
              return true;
            }

            auto synchronization_data = synchronization_data_watcher.lock();
            auto export_data          = export_data_watcher.lock();
            {
              std::unique_lock<std::mutex> lk(export_data->async_export_data_m);
              if (export_data->export_ids_flag[id - 1] == false)
              {
                export_data->export_ids.push(id);
                export_data->export_ids_flag[id - 1] = true;
              }
            }
            NotifyCompletion(notify_force_flush, synchronization_data, export_data);
            return true;
          });
    }
  } while (true);
}

void AsyncBatchSpanProcessor::NotifyCompletion(
    bool notify_force_flush,
    const std::shared_ptr<SynchronizationData> &synchronization_data,
    const std::shared_ptr<ExportDataStorage> &export_data_storage)
{
  BatchSpanProcessor::NotifyCompletion(notify_force_flush, synchronization_data);
  export_data_storage->async_export_waker.notify_all();
}

bool AsyncBatchSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  auto start_time = std::chrono::system_clock::now();
  std::lock_guard<std::mutex> shutdown_guard{synchronization_data_->shutdown_m};
  bool already_shutdown = synchronization_data_->is_shutdown.exchange(true);

  if (worker_thread_.joinable())
  {
    synchronization_data_->is_force_wakeup_background_worker.store(true, std::memory_order_release);
    synchronization_data_->cv.notify_one();
    worker_thread_.join();
  }

  GetWaitAdjustedTime(timeout, start_time);
  // wait for  all async exports to complete and return if timeout reached.
  {
    std::unique_lock<std::mutex> lock(export_data_storage_->async_export_data_m);
    export_data_storage_->async_export_waker.wait_for(lock, timeout, [this] {
      return export_data_storage_->export_ids.size() == max_export_async_;
    });
  }

  GetWaitAdjustedTime(timeout, start_time);
  // Should only shutdown exporter ONCE.
  if (!already_shutdown && exporter_ != nullptr)
  {
    return exporter_->Shutdown(timeout);
  }

  return true;
}

AsyncBatchSpanProcessor::~AsyncBatchSpanProcessor() {}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
