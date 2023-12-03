// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/logs/batch_log_record_processor.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/sdk/logs/recordable.h"

#include <vector>

using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBufferRange;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
BatchLogRecordProcessor::BatchLogRecordProcessor(
    std::unique_ptr<LogRecordExporter> &&exporter,
    const size_t max_queue_size,
    const std::chrono::milliseconds scheduled_delay_millis,
    const size_t max_export_batch_size)
    : exporter_(std::move(exporter)),
      max_queue_size_(max_queue_size),
      scheduled_delay_millis_(scheduled_delay_millis),
      max_export_batch_size_(max_export_batch_size),
      buffer_(max_queue_size_),
      synchronization_data_(std::make_shared<SynchronizationData>()),
      worker_thread_(&BatchLogRecordProcessor::DoBackgroundWork, this)
{}

BatchLogRecordProcessor::BatchLogRecordProcessor(std::unique_ptr<LogRecordExporter> &&exporter,
                                                 const BatchLogRecordProcessorOptions &options)
    : exporter_(std::move(exporter)),
      max_queue_size_(options.max_queue_size),
      scheduled_delay_millis_(options.schedule_delay_millis),
      max_export_batch_size_(options.max_export_batch_size),
      buffer_(options.max_queue_size),
      synchronization_data_(std::make_shared<SynchronizationData>()),
      worker_thread_(&BatchLogRecordProcessor::DoBackgroundWork, this)
{}

std::unique_ptr<Recordable> BatchLogRecordProcessor::MakeRecordable() noexcept
{
  return exporter_->MakeRecordable();
}

void BatchLogRecordProcessor::OnEmit(std::unique_ptr<Recordable> &&record) noexcept
{
  if (synchronization_data_->is_shutdown.load() == true)
  {
    return;
  }

  if (buffer_.Add(std::unique_ptr<Recordable>(record.release())) == false)
  {
    return;
  }

  // If the queue gets at least half full a preemptive notification is
  // sent to the worker thread to start a new export cycle.
  size_t buffer_size = buffer_.size();
  if (buffer_size >= max_queue_size_ / 2 || buffer_size >= max_export_batch_size_)
  {
    // signal the worker thread
    synchronization_data_->is_force_wakeup_background_worker.store(true, std::memory_order_release);
    synchronization_data_->cv.notify_one();
  }
}

bool BatchLogRecordProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  if (synchronization_data_->is_shutdown.load() == true)
  {
    return false;
  }

  // Now wait for the worker thread to signal back from the Export method
  std::unique_lock<std::mutex> lk_cv(synchronization_data_->force_flush_cv_m);

  synchronization_data_->is_force_flush_pending.store(true, std::memory_order_release);
  synchronization_data_->force_flush_timeout_us.store(timeout.count(), std::memory_order_release);
  auto break_condition = [this]() {
    if (synchronization_data_->is_shutdown.load() == true)
    {
      return true;
    }

    // Wake up the worker thread once.
    if (synchronization_data_->is_force_flush_pending.load(std::memory_order_acquire))
    {
      synchronization_data_->cv.notify_one();
    }

    return synchronization_data_->is_force_flush_notified.load(std::memory_order_acquire);
  };

  // Fix timeout to meet requirement of wait_for
  timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());

  std::chrono::steady_clock::duration timeout_steady =
      std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout);
  if (timeout_steady <= std::chrono::steady_clock::duration::zero())
  {
    timeout_steady = (std::chrono::steady_clock::duration::max)();
  }

  bool result = false;
  while (!result && timeout_steady > std::chrono::steady_clock::duration::zero())
  {
    // When is_force_flush_notified.store(true) and force_flush_cv.notify_all() is called
    // between is_force_flush_pending.load() and force_flush_cv.wait(). We must not wait
    // for ever
    std::chrono::steady_clock::time_point start_timepoint = std::chrono::steady_clock::now();
    result = synchronization_data_->force_flush_cv.wait_for(lk_cv, scheduled_delay_millis_,
                                                            break_condition);
    timeout_steady -= std::chrono::steady_clock::now() - start_timepoint;
  }

  // If it's already signaled, we must wait util notified.
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

void BatchLogRecordProcessor::DoBackgroundWork()
{
  auto timeout = scheduled_delay_millis_;

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
    timeout = scheduled_delay_millis_ - duration;
  }
}

void BatchLogRecordProcessor::Export()
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
      NotifyCompletion(notify_force_flush, exporter_, synchronization_data_);
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

    exporter_->Export(
        nostd::span<std::unique_ptr<Recordable>>(records_arr.data(), records_arr.size()));
    NotifyCompletion(notify_force_flush, exporter_, synchronization_data_);
  } while (true);
}

void BatchLogRecordProcessor::NotifyCompletion(
    bool notify_force_flush,
    const std::unique_ptr<LogRecordExporter> &exporter,
    const std::shared_ptr<SynchronizationData> &synchronization_data)
{
  if (!synchronization_data)
  {
    return;
  }

  if (notify_force_flush)
  {
    if (exporter)
    {
      std::chrono::microseconds timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
          std::chrono::microseconds{
              synchronization_data->force_flush_timeout_us.load(std::memory_order_acquire)},
          std::chrono::microseconds::zero());
      exporter->ForceFlush(timeout);
    }
    synchronization_data->is_force_flush_notified.store(true, std::memory_order_release);
    synchronization_data->force_flush_cv.notify_one();
  }
}

void BatchLogRecordProcessor::DrainQueue()
{
  while (true)
  {
    if (buffer_.empty() &&
        false == synchronization_data_->is_force_flush_pending.load(std::memory_order_acquire))
    {
      break;
    }

    Export();
  }
}

void BatchLogRecordProcessor::GetWaitAdjustedTime(
    std::chrono::microseconds &timeout,
    std::chrono::time_point<std::chrono::system_clock> &start_time)
{
  auto end_time = std::chrono::system_clock::now();
  auto offset   = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  start_time    = end_time;
  timeout       = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
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
}

bool BatchLogRecordProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
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
  // Should only shutdown exporter ONCE.
  if (!already_shutdown && exporter_ != nullptr)
  {
    return exporter_->Shutdown(timeout);
  }

  return true;
}

BatchLogRecordProcessor::~BatchLogRecordProcessor()
{
  if (synchronization_data_->is_shutdown.load() == false)
  {
    Shutdown();
  }
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
