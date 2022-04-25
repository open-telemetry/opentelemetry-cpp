// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  ifdef ENABLE_ASYNC_EXPORT
#    include "opentelemetry/sdk/logs/async_batch_log_processor.h"
#    include "opentelemetry/common/spin_lock_mutex.h"

#    include <vector>
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBufferRange;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
AsyncBatchLogProcessor::AsyncBatchLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                                               const AsyncBatchLogProcessorOptions &options)
    : BatchLogProcessor(std::move(exporter), options),
      max_export_async_(options.max_export_async),
      export_data_storage_(std::make_shared<ExportDataStorage>())
{
  export_data_storage_->export_ids_flag.resize(max_export_async_, true);
  for (int i = 1; i <= max_export_async_; i++)
  {
    export_data_storage_->export_ids.push(i);
  }
}

void AsyncBatchLogProcessor::Export()
{
  do
  {
    std::vector<std::unique_ptr<Recordable>> records_arr;
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
                        records_arr.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
                        return true;
                      });
                    });

    size_t id = kInvalidExportId;
    {
      std::unique_lock<std::mutex> lock(export_data_storage_->async_export_data_m);
      export_data_storage_->async_export_waker.wait_for(lock, scheduled_delay_millis_, [this] {
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
      std::weak_ptr<ExportDataStorage> export_data_watcher            = export_data_storage_;
      std::weak_ptr<SynchronizationData> synchronization_data_watcher = synchronization_data_;
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(records_arr.data(), records_arr.size()),
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
            bool is_already_notified  = false;
            auto synchronization_data = synchronization_data_watcher.lock();
            auto export_data          = export_data_watcher.lock();
            {
              std::unique_lock<std::mutex> lk(export_data->async_export_data_m);
              // In case callback is called more than once due to some bug in exporter
              // we need to ensure export_ids do not contain duplicate.
              if (export_data->export_ids_flag[id - 1] == false)
              {
                export_data->export_ids.push(id);
                export_data->export_ids_flag[id - 1] = true;
              }
              else
              {
                is_already_notified = true;
              }
            }
            if (is_already_notified == false)
            {
              NotifyCompletion(notify_force_flush, synchronization_data, export_data);
            }
            return true;
          });
    }
  } while (true);
}

void AsyncBatchLogProcessor::NotifyCompletion(
    bool notify_force_flush,
    const std::shared_ptr<SynchronizationData> &synchronization_data,
    const std::shared_ptr<ExportDataStorage> &export_data_storage)
{
  BatchLogProcessor::NotifyCompletion(notify_force_flush, synchronization_data);
  export_data_storage->async_export_waker.notify_all();
}

bool AsyncBatchLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
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

AsyncBatchLogProcessor::~AsyncBatchLogProcessor()
{
  if (synchronization_data_->is_shutdown.load() == false)
  {
    Shutdown();
  }
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#  endif
#endif
