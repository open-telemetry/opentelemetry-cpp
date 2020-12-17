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

using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBufferRange;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/************************** Constructor *******************************/

BatchLogProcessor::BatchLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                                     const size_t max_queue_size,
                                     const std::chrono::milliseconds scheduled_delay_millis,
                                     const size_t max_export_batch_size)
    : exporter_(std::move(exporter)),
      max_queue_size_(max_queue_size),
      scheduled_delay_millis_(scheduled_delay_millis),
      max_export_batch_size_(max_export_batch_size),
      buffer_(max_queue_size_),
      worker_thread_(&BatchLogProcessor::DoBackgroundWork, this)
{}

/************************** Helper methods used by the worker thread ***************/

void BatchLogProcessor::Export(const bool was_force_flush_called)
{
  // Figure out the number of records to export
  size_t num_records_to_export;

  if (was_force_flush_called == true)
  {
    num_records_to_export = buffer_.size();
  }
  else
  {
    // Export the min of max_export_batch_size and buffer_size
    num_records_to_export =
        buffer_.size() >= max_export_batch_size_ ? max_export_batch_size_ : buffer_.size();
  }

  // Get records from the circular buffer and put into a vector
  std::vector<std::unique_ptr<Recordable>> records_arr;

  buffer_.Consume(
      num_records_to_export, [&](CircularBufferRange<AtomicUniquePtr<Recordable>> range) noexcept {
        range.ForEach([&](AtomicUniquePtr<Recordable> &ptr) {
          std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
          ptr.Swap(swap_ptr);
          records_arr.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
          return true;
        });
      });

  // Call exporter with the records, and get export status
  if (exporter_->Export(nostd::span<std::unique_ptr<Recordable>>(
          records_arr.data(), records_arr.size())) != ExportResult::kSuccess)
  {
    // Indicate Error: "[Batch Log Processor]: Failed to export a batch"
  }

  // In case this export was the result of a force flush, notify main thread that force flush is
  // complete
  if (was_force_flush_called == true)
  {
    is_force_flush_notified_.store(true);

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

/**
 * Note this method will not be called concurrently, as it is only called once in the processor's
 * constructor
 */
void BatchLogProcessor::DoBackgroundWork()
{
  auto timeout = scheduled_delay_millis_;

  while (true)
  {
    // Wait for `timeout` milliseconds
    std::unique_lock<std::mutex> lk(cv_m_);
    cv_.wait_for(lk, timeout);

    // Check if shutown was called, drain queue
    if (is_shutdown_.load() == true)
    {
      DrainQueue();
      return;
    }

    // Check if ForceFlush was called, notify main thread that worker thread has been notified
    bool was_force_flush_called = is_force_flush_.exchange(false);

    // If the buffer was empty during the entire `timeout` time interval,
    // go back to waiting. If this was a spurious wake-up, we export only if
    // `buffer_` is not empty. This is acceptable because batching is a best
    // mechanism effort here.
    if (buffer_.empty() == true)
    {
      continue;
    }

    auto start = std::chrono::steady_clock::now();

    Export(was_force_flush_called);

    auto end      = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Subtract the duration of this export call from the next `timeout`.
    timeout = scheduled_delay_millis_ - duration;
  }
}

/****************************** Overloaded processor methods ******************************/

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
  if (buffer_.size() >= max_queue_size_ / 2)
  {
    // signal the worker thread
    cv_.notify_one();
  }
}

/**
 * Notifies the worker thread that ForceFlush() has been called,
 * and waits for worker to complete the force flush.
 *
 * Note: timeout functionality is currently not implemented.
 */
bool BatchLogProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  if (is_shutdown_.load() == true)
  {
    return false;
  }

  is_force_flush_.store(true);

  // Keep attempting to notify the worker thread that force flush has been caled
  while (is_force_flush_.load() == true)
  {
    cv_.notify_one();
  }

  // Wait for the worker thread complete the force flush in the Export method
  std::unique_lock<std::mutex> lk(force_flush_cv_m_);
  force_flush_cv_.wait(lk, [this] { return is_force_flush_notified_.load(); });

  // Notify the worker thread
  is_force_flush_notified_.store(false);
  return true;
}

/**
 * Drains the current buffer of logs before shutting down.
 * Note: Timeout functionality is currently not implemented
 */
bool BatchLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  std::unique_lock<std::mutex> lock(shutdown_mutex_);

  if (is_shutdown_.exchange(true) == false)
  {
    // Notifies worker thread that shutdown has been called
    cv_.notify_one();
    // Wait until worker thread completes current export
    worker_thread_.join();

    // Shuts down the exporter
    if (exporter_ != nullptr)
    {
      return exporter_->Shutdown();
    }
    return true;
  }
  return false;
}

BatchLogProcessor::~BatchLogProcessor()
{
  Shutdown();
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
