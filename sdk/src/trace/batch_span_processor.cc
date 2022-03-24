// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/common/spin_lock_mutex.h"

#include <vector>
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;
using opentelemetry::trace::SpanContext;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

BatchSpanProcessor::BatchSpanProcessor(std::unique_ptr<SpanExporter> &&exporter,
                                       const BatchSpanProcessorOptions &options)
    : exporter_(std::move(exporter)),
      max_queue_size_(options.max_queue_size),
      schedule_delay_millis_(options.schedule_delay_millis),
      max_export_batch_size_(options.max_export_batch_size),
      is_export_async_(options.is_export_async),
      buffer_(max_queue_size_),
      synchronization_data_(std::make_shared<SynchronizationData>()),
      worker_thread_(&BatchSpanProcessor::DoBackgroundWork, this)
{
  synchronization_data_->is_force_wakeup_background_worker.store(false);
  synchronization_data_->is_force_flush_pending.store(false);
  synchronization_data_->is_force_flush_notified.store(false);
  synchronization_data_->is_shutdown.store(false);
  synchronization_data_->is_async_shutdown_notified.store(false);
}

std::unique_ptr<Recordable> BatchSpanProcessor::MakeRecordable() noexcept
{
  return exporter_->MakeRecordable();
}

void BatchSpanProcessor::OnStart(Recordable &, const SpanContext &) noexcept
{
  // no-op
}

void BatchSpanProcessor::OnEnd(std::unique_ptr<Recordable> &&span) noexcept
{
  if (synchronization_data_->is_shutdown.load() == true)
  {
    return;
  }

  if (buffer_.Add(span) == false)
  {
    return;
  }

  // If the queue gets at least half full a preemptive notification is
  // sent to the worker thread to start a new export cycle.
  size_t buffer_size = buffer_.size();
  if (buffer_size >= max_queue_size_ / 2 || buffer_size >= max_export_batch_size_)
  {
    // signal the worker thread
    synchronization_data_->cv.notify_one();
  }
}

bool BatchSpanProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  if (synchronization_data_->is_shutdown.load() == true)
  {
    return false;
  }

  // Now wait for the worker thread to signal back from the Export method
  std::unique_lock<std::mutex> lk_cv(synchronization_data_->force_flush_cv_m);

  synchronization_data_->is_force_flush_pending.store(true, std::memory_order_release);
  auto break_condition = [this]() {
    if (synchronization_data_->is_shutdown.load() == true)
    {
      return true;
    }

    // Wake up the worker thread once.
    if (synchronization_data_->is_force_flush_pending.load(std::memory_order_acquire))
    {
      synchronization_data_->is_force_wakeup_background_worker.store(true,
                                                                     std::memory_order_release);
      synchronization_data_->cv.notify_one();
    }

    return synchronization_data_->is_force_flush_notified.load(std::memory_order_acquire);
  };

  // Fix timeout to meet requirement of wait_for
  timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());
  bool result;
  if (timeout <= std::chrono::microseconds::zero())
  {
    bool wait_result = false;
    while (!wait_result)
    {
      // When is_force_flush_notified.store(true) and force_flush_cv.notify_all() is called
      // between is_force_flush_pending.load() and force_flush_cv.wait(). We must not wait
      // for ever
      wait_result = synchronization_data_->force_flush_cv.wait_for(lk_cv, schedule_delay_millis_,
                                                                   break_condition);
    }
    result = true;
  }
  else
  {
    result = synchronization_data_->force_flush_cv.wait_for(lk_cv, timeout, break_condition);
  }

  // If it will be already signaled, we must wait util notified.
  // We use a spin lock here
  if (false ==
      synchronization_data_->is_force_flush_pending.exchange(false, std::memory_order_acq_rel))
  {
    for (int retry_waiting_times = 0;
         false == synchronization_data_->is_force_flush_notified.load(std::memory_order_acquire);
         ++retry_waiting_times)
    {
      opentelemetry::common::SpinLockMutex::fast_yield();
      if ((retry_waiting_times & 127) == 127)
      {
        std::this_thread::yield();
      }
    }
  }
  synchronization_data_->is_force_flush_notified.store(false, std::memory_order_release);

  return result;
}

void BatchSpanProcessor::DoBackgroundWork()
{
  auto timeout = schedule_delay_millis_;

  while (true)
  {
    // Wait for `timeout` milliseconds
    std::unique_lock<std::mutex> lk(synchronization_data_->cv_m);
    synchronization_data_->cv.wait_for(lk, timeout, [this] {
      if (synchronization_data_->is_force_wakeup_background_worker.load(std::memory_order_acquire))
      {
        return true;
      }

      return !buffer_.empty();
    });
    synchronization_data_->is_force_wakeup_background_worker.store(false,
                                                                   std::memory_order_release);

    if (synchronization_data_->is_shutdown.load() == true)
    {
      DrainQueue();
      return;
    }

    auto start = std::chrono::steady_clock::now();
    Export();
    auto end      = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Subtract the duration of this export call from the next `timeout`.
    timeout = schedule_delay_millis_ - duration;
  }
}

void BatchSpanProcessor::Export()
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
      NotifyCompletion(notify_force_flush, synchronization_data_);
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

    /* Call the sync Export when force flush was called, even if
       is_export_async_ is true.
    */
    if (is_export_async_ == false)
    {
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()));
      NotifyCompletion(notify_force_flush, synchronization_data_);
    }
    else
    {
      std::weak_ptr<SynchronizationData> synchronization_data_watcher = synchronization_data_;
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()),
          [notify_force_flush, synchronization_data_watcher](sdk::common::ExportResult result) {
            // TODO: Print result
            if (synchronization_data_watcher.expired())
            {
              return true;
            }

            NotifyCompletion(notify_force_flush, synchronization_data_watcher.lock());
            return true;
          });
    }
  } while (true);
}

void BatchSpanProcessor::WaitForShutdownCompletion()
{
  // Since async export is invoked due to shutdown, need to wait
  // for async thread to complete.
  if (is_export_async_)
  {
    std::unique_lock<std::mutex> lk(synchronization_data_->async_shutdown_m);
    while (true)
    {
      if (synchronization_data_->is_async_shutdown_notified.load())
      {
        break;
      }

      // When is_async_shutdown_notified.store(true) and async_shutdown_cv.notify_all() is called
      // between is_async_shutdown_notified.load() and async_shutdown_cv.wait(). We must not wait
      // for ever
      synchronization_data_->async_shutdown_cv.wait_for(lk, schedule_delay_millis_);
    }
  }
}

void BatchSpanProcessor::NotifyCompletion(
    bool notify_force_flush,
    const std::shared_ptr<SynchronizationData> &synchronization_data)
{
  if (!synchronization_data)
  {
    return;
  }

  if (notify_force_flush)
  {
    synchronization_data->is_force_flush_notified.store(true, std::memory_order_release);
    synchronization_data->force_flush_cv.notify_one();
  }

  // Notify the thread which is waiting on shutdown to complete.
  if (synchronization_data->is_shutdown.load() == true)
  {
    synchronization_data->is_async_shutdown_notified.store(true);
    synchronization_data->async_shutdown_cv.notify_all();
  }
}

void BatchSpanProcessor::DrainQueue()
{
  while (true)
  {
    if (buffer_.empty() &&
        false == synchronization_data_->is_force_flush_pending.load(std::memory_order_acquire))
    {
      break;
    }

    Export();
    WaitForShutdownCompletion();
  }
}

bool BatchSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
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

  auto worker_end_time = std::chrono::system_clock::now();
  auto offset = std::chrono::duration_cast<std::chrono::microseconds>(worker_end_time - start_time);

  // Fix timeout to meet requirement of wait_for
  timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());
  if (timeout > offset && timeout > std::chrono::microseconds::zero())
  {
    timeout -= offset;
  }
  else
  {
    // Some module use zero as indefinite timeout.So we can not reset timeout to zero here
    timeout = std::chrono::microseconds(1);
  }

  // Should only shutdown exporter ONCE.
  if (!already_shutdown && exporter_ != nullptr)
  {
    return exporter_->Shutdown(timeout);
  }

  return true;
}

BatchSpanProcessor::~BatchSpanProcessor()
{
  if (synchronization_data_->is_shutdown.load() == false)
  {
    Shutdown();
  }
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
