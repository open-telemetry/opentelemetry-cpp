/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "opentelemetry/sdk/logs/batch_log_processor.h"

#include <iostream>
#include <vector>

using opentelemetry::logs::LogRecord;
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
BatchLogProcessor::BatchLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                                     const size_t max_queue_size,
                                     const std::chrono::milliseconds schedule_delay_millis,
                                     const size_t max_export_batch_size)
    : exporter_(std::move(exporter)),
      max_queue_size_(max_queue_size),
      schedule_delay_millis_(schedule_delay_millis),
      max_export_batch_size_(max_export_batch_size),
      buffer_(max_queue_size_),
      worker_thread_(&BatchLogProcessor::DoBackgroundWork, this)
{}

void BatchLogProcessor::OnReceive(std::unique_ptr<LogRecord> &&record) noexcept
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
  if (buffer_.size() >= max_queue_size_ / 2)
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

  is_force_flush_ = true;

  // Keep attempting to wake up the worker thread
  while (is_force_flush_.load() == true)
  {
    cv_.notify_one();
  }

  // Now wait for the worker thread to signal back from the Export method
  std::unique_lock<std::mutex> lk(force_flush_cv_m_);
  while (is_force_flush_notified_.load() == false)
  {
    force_flush_cv_.wait(lk);
  }

  // Notify the worker thread
  is_force_flush_notified_ = false;
  return true;
}

void BatchLogProcessor::DoBackgroundWork()
{
  auto timeout = schedule_delay_millis_;

  while (true)
  {
    // Wait for `timeout` milliseconds
    std::unique_lock<std::mutex> lk(cv_m_);
    cv_.wait_for(lk, timeout);

    if (is_shutdown_.load() == true)
    {
      DrainQueue();
      return;
    }

    bool was_force_flush_called = is_force_flush_.load();

    // Check if this export was the result of a force flush.
    if (was_force_flush_called == true)
    {
      // Since this export was the result of a force flush, signal the
      // main thread that the worker thread has been notified
      is_force_flush_ = false;
    }
    else
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
    timeout = schedule_delay_millis_ - duration;
  }
}

void BatchLogProcessor::Export(const bool was_force_flush_called)
{
  std::vector<std::unique_ptr<LogRecord>> records_arr;

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

  buffer_.Consume(
      num_records_to_export, [&](CircularBufferRange<AtomicUniquePtr<LogRecord>> range) noexcept {
        range.ForEach([&](AtomicUniquePtr<LogRecord> &ptr) {
          std::unique_ptr<LogRecord> swap_ptr = std::unique_ptr<LogRecord>(nullptr);
          ptr.Swap(swap_ptr);
          records_arr.push_back(std::unique_ptr<LogRecord>(swap_ptr.release()));
          return true;
        });
      });

  ExportResult export_status = exporter_->Export(records_arr);
  if (export_status != ExportResult::kSuccess)
  {
    // Error
  }

  // Notify the main thread in case this export was the result of a force flush.
  if (was_force_flush_called == true)
  {
    is_force_flush_notified_ = true;
    while (is_force_flush_notified_.load() == true)
    {
      force_flush_cv_.notify_one();
    }
  }
}

void BatchLogProcessor::DrainQueue()
{
  while (buffer_.empty() == false)
  {
    Export(false);
  }
}

// Note: Timeout functionality is currently not implemented
bool BatchLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  if (is_shutdown_.load() == true)
  {
    return false;
  }

  is_shutdown_ = true;

  // notifies worker thread that shutdown has been called
  cv_.notify_one();
  // wait until worker thread completes current export
  worker_thread_.join();
  // calls the exporter to shutdown
  if (exporter_ != nullptr)
  {
    return exporter_->Shutdown();
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
