// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/batch_span_processor.h"

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
      buffer_(max_queue_size_),
      is_export_async_(options.is_export_async),
      worker_thread_(&BatchSpanProcessor::DoBackgroundWork, this)
{
  is_shutdown_.store(false);
  is_force_flush_.store(false);
  is_force_flush_notified_.store(false);
  is_async_shutdown_notified_.store(false);
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
  if (is_shutdown_.load() == true)
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
    cv_.notify_one();
  }
}

bool BatchSpanProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
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

void BatchSpanProcessor::DoBackgroundWork()
{
  auto timeout = schedule_delay_millis_;

  while (true)
  {
    // Wait for `timeout` milliseconds
    std::unique_lock<std::mutex> lk(cv_m_);
    cv_.wait_for(lk, timeout);

    if (is_shutdown_.load() == true)
    {
      // Break loop if another thread call ForceFlush
      is_force_flush_ = false;
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
        timeout = schedule_delay_millis_;
        continue;
      }
    }

    auto start = std::chrono::steady_clock::now();
    Export(was_force_flush_called);
    auto end      = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Subtract the duration of this export call from the next `timeout`.
    timeout = schedule_delay_millis_ - duration;
  }
}

void BatchSpanProcessor::Export(const bool was_force_flush_called)
{
  do
  {
    std::vector<std::unique_ptr<Recordable>> spans_arr;
    size_t num_spans_to_export;

    if (was_force_flush_called == true)
    {
      num_spans_to_export = buffer_.size();
    }
    else
    {
      num_spans_to_export =
          buffer_.size() >= max_export_batch_size_ ? max_export_batch_size_ : buffer_.size();
    }

    if (num_spans_to_export == 0)
    {
      break;
    }
    buffer_.Consume(num_spans_to_export,
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
    }
    else
    {
      exporter_->Export(
          nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()),
          [this, was_force_flush_called](sdk::common::ExportResult result) {
            // TODO: Print result
            NotifyForceFlushCompletion(was_force_flush_called);
            // If export was called due to shutdown, notify the worker thread
            NotifyShutdownCompletion();
            return true;
          });
    }
  } while (was_force_flush_called);

  if (is_export_async_ == false)
  {
    NotifyForceFlushCompletion(was_force_flush_called);
  }
}

void BatchSpanProcessor::NotifyForceFlushCompletion(const bool was_force_flush_called)
{
  // Notify the main thread in case this export was the result of a force flush.
  if (was_force_flush_called == true)
  {
    if (is_force_flush_notified_.exchange(true, std::memory_order_acq_rel) == false)
    {
      force_flush_cv_.notify_all();
    }
  }
}

void BatchSpanProcessor::WaitForShutdownCompletion()
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

void BatchSpanProcessor::NotifyShutdownCompletion()
{
  // Notify the thread which is waiting on shutdown to complete.
  if (is_shutdown_.load() == true)
  {
    is_async_shutdown_notified_.store(true);
    async_shutdown_cv_.notify_one();
  }
}

void BatchSpanProcessor::DrainQueue()
{
  while (buffer_.empty() == false)
  {
    Export(false);
    WaitForShutdownCompletion();
  }
}

bool BatchSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
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
  if (is_shutdown_.load() == false)
  {
    Shutdown();
  }
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
