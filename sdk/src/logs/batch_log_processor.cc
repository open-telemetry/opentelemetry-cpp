// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  include "opentelemetry/sdk/logs/batch_log_processor.h"

#  include <vector>
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBufferRange;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
BatchLogProcessor::BatchLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                                     const size_t max_queue_size,
                                     const std::chrono::milliseconds scheduled_delay_millis,
                                     const size_t max_export_batch_size,
                                     const bool is_export_async)
    : exporter_(std::move(exporter)),
      max_queue_size_(max_queue_size),
      scheduled_delay_millis_(scheduled_delay_millis),
      max_export_batch_size_(max_export_batch_size),
      is_export_async_(is_export_async),
      buffer_(max_queue_size_),
      worker_thread_(&BatchLogProcessor::DoBackgroundWork, this)
{
  is_shutdown_.store(false);
  is_force_flush_.store(false);
  is_force_flush_notified_.store(false);
  is_async_shutdown_notified_.store(false);
}

BatchLogProcessor::BatchLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                                     const BatchLogProcessorOptions &options)
    : exporter_(std::move(exporter)),
      max_queue_size_(options.max_queue_size),
      scheduled_delay_millis_(options.schedule_delay_millis),
      max_export_batch_size_(options.max_export_batch_size),
      is_export_async_(options.is_export_async),
      buffer_(options.max_queue_size),
      worker_thread_(&BatchLogProcessor::DoBackgroundWork, this)
{
  is_shutdown_.store(false);
  is_force_flush_.store(false);
  is_force_flush_notified_.store(false);
  is_async_shutdown_notified_.store(false);
}

std::unique_ptr<Recordable> BatchLogProcessor::MakeRecordable() noexcept
{
  return exporter_->MakeRecordable();
}

void BatchLogProcessor::OnReceive(std::unique_ptr<Recordable> &&record) noexcept
{
  if (is_shutdown_.load() == true)
  {
    return;
  }

  if (buffer_.Add(record) == false)
  {
    return;
  }

  // If the queue gets at least half full a preemptive notification is
  // sent to the worker thread to start a new export cycle.
  size_t buffer_size = buffer_.size();
  if (buffer_size >= max_queue_size_ / 2 || buffer_size >= max_export_batch_size_)
  {
    // signal the worker thread
    cv_.notify_one();
  }
}

bool BatchLogProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  if (is_shutdown_.load() == true)
  {
    return false;
  }

  // Now wait for the worker thread to signal back from the Export method
  std::unique_lock<std::mutex> lk(force_flush_cv_m_);

  is_force_flush_notified_.store(false, std::memory_order_release);
  auto break_condition = [this]() {
    if (is_shutdown_.load() == true)
    {
      return true;
    }

    // Wake up the worker thread once.
    if (is_force_flush_.exchange(true) == false)
    {
      cv_.notify_one();
    }

    return is_force_flush_notified_.load(std::memory_order_acquire);
  };

  // Fix timeout to meet requirement of wait_for
  timeout = opentelemetry::common::DurationUtil::AdjustWaitForTimeout(
      timeout, std::chrono::microseconds::zero());
  if (timeout <= std::chrono::microseconds::zero())
  {
    force_flush_cv_.wait(lk, break_condition);
    return true;
  }
  else
  {
    return force_flush_cv_.wait_for(lk, timeout, break_condition);
  }
}

void BatchLogProcessor::DoBackgroundWork()
{
  auto timeout = scheduled_delay_millis_;

  while (true)
  {
    // Wait for `timeout` milliseconds
    std::unique_lock<std::mutex> lk(cv_m_);
    cv_.wait_for(lk, timeout);

    if (is_shutdown_.load() == true)
    {
      // Break loop if another thread call ForceFlush
      DrainQueue();
      return;
    }

    bool was_force_flush_called = is_force_flush_.exchange(false);

    // Check if this export was the result of a force flush.
    if (!was_force_flush_called)
    {
      // If the buffer was empty during the entire `timeout` time interval,
      // go back to waiting. If this was a spurious wake-up, we export only if
      // `buffer_` is not empty. This is acceptable because batching is a best
      // mechanism effort here.
      if (buffer_.empty() == true)
      {
        continue;
      }
    }

    auto start = std::chrono::steady_clock::now();
    Export(was_force_flush_called);
    auto end      = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Subtract the duration of this export call from the next `timeout`.
    timeout = scheduled_delay_millis_ - duration;
  }
}

void BatchLogProcessor::Export(const bool was_force_flush_called)
{
  bool notify_force_completion = true;
  do
  {
    std::vector<std::unique_ptr<Recordable>> records_arr;
    size_t num_records_to_export;

    if (was_force_flush_called == true)
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

    if (is_export_async_ == false)
    {
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(records_arr.data(), records_arr.size()));
    }
    else
    {
      notify_force_completion = false;
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(records_arr.data(), records_arr.size()),
          [this, was_force_flush_called](sdk::common::ExportResult result) {
            // TODO: Print result
            if (was_force_flush_called)
            {
              NotifyForceFlushCompletion();
            }

            // Notify the thread which is waiting on shutdown to complete.
            NotifyShutdownCompletion();
            return true;
          });
    }
  } while (was_force_flush_called);

  if (notify_force_completion)
  {
    if (was_force_flush_called)
    {
      NotifyForceFlushCompletion();
    }
    // Notify the thread which is waiting on shutdown to complete.
    NotifyShutdownCompletion();
  }
}

void BatchLogProcessor::NotifyForceFlushCompletion()
{
  // Notify the main thread in case this export was the result of a force flush.
  if (is_force_flush_notified_.exchange(true, std::memory_order_acq_rel) == false)
  {
    force_flush_cv_.notify_all();
  }
}

void BatchLogProcessor::WaitForShutdownCompletion()
{
  // Since async export is invoked due to shutdown, need to wait
  // for async thread to complete.
  if (is_export_async_)
  {
    std::unique_lock<std::mutex> lk(async_shutdown_m_);
    while (is_async_shutdown_notified_.load() == false)
    {
      async_shutdown_cv_.wait(lk);
    }
  }
}

void BatchLogProcessor::NotifyShutdownCompletion()
{
  // Notify the thread which is waiting on shutdown to complete.
  if (is_shutdown_.load() == true)
  {
    is_async_shutdown_notified_.store(true);
    async_shutdown_cv_.notify_all();
  }
}

void BatchLogProcessor::DrainQueue()
{
  while (true)
  {
    bool was_force_flush_called = is_force_flush_.exchange(false);
    if (buffer_.empty() && !was_force_flush_called)
    {
      break;
    }

    Export(was_force_flush_called);

    // Since async export is invoked due to shutdown, need to wait
    // for async thread to complete.
    WaitForShutdownCompletion();
  }
}

bool BatchLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  auto start_time = std::chrono::system_clock::now();

  std::lock_guard<std::mutex> shutdown_guard{shutdown_m_};
  bool already_shutdown = is_shutdown_.exchange(true);

  if (worker_thread_.joinable())
  {
    cv_.notify_one();
    worker_thread_.join();
  }

  auto worker_end_time = std::chrono::system_clock::now();
  auto offset = std::chrono::duration_cast<std::chrono::microseconds>(worker_end_time - start_time);

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

BatchLogProcessor::~BatchLogProcessor()
{
  if (is_shutdown_.load() == false)
  {
    Shutdown();
  }
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
