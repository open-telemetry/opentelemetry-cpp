#include "opentelemetry/sdk/trace/fork_aware_batch_span_processor.h"

#include <iostream>
#include <vector>
using opentelemetry::sdk::common::AtomicUniquePtr;
using opentelemetry::sdk::common::CircularBuffer;
using opentelemetry::sdk::common::CircularBufferRange;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
ForkAwareBatchSpanProcessor::ForkAwareBatchSpanProcessor(
    std::unique_ptr<SpanExporter> &&exporter,
    const size_t max_queue_size,
    const std::chrono::milliseconds schedule_delay_millis,
    const size_t max_export_batch_size)
    : exporter_(std::move(exporter)),
      max_queue_size_(max_queue_size),
      schedule_delay_millis_(schedule_delay_millis),
      max_export_batch_size_(max_export_batch_size),
      buffer_(max_queue_size_),
      worker_thread_(&ForkAwareBatchSpanProcessor::DoBackgroundWork, this)
{}

std::unique_ptr<Recordable> ForkAwareBatchSpanProcessor::MakeRecordable() noexcept
{
  return exporter_->MakeRecordable();
}

void ForkAwareBatchSpanProcessor::OnStart(Recordable &) noexcept
{
  // no-op
}

void ForkAwareBatchSpanProcessor::OnEnd(std::unique_ptr<Recordable> &&span) noexcept
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
  if (buffer_.size() >= max_queue_size_ / 2)
  {
    // signal the worker thread
    cv_.notify_one();
  }
}

void ForkAwareBatchSpanProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  if (is_shutdown_.load() == true)
  {
    return;
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
}

void ForkAwareBatchSpanProcessor::DoBackgroundWork()
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

void ForkAwareBatchSpanProcessor::Export(const bool was_force_flush_called)
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

  buffer_.Consume(
      num_spans_to_export, [&](CircularBufferRange<AtomicUniquePtr<Recordable>> range) noexcept {
        range.ForEach([&](AtomicUniquePtr<Recordable> &ptr) {
          std::unique_ptr<Recordable> swap_ptr = std::unique_ptr<Recordable>(nullptr);
          ptr.Swap(swap_ptr);
          spans_arr.push_back(std::unique_ptr<Recordable>(swap_ptr.release()));
          return true;
        });
      });

  exporter_->Export(nostd::span<std::unique_ptr<Recordable>>(spans_arr.data(), spans_arr.size()));

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

void ForkAwareBatchSpanProcessor::DrainQueue()
{
  while (buffer_.empty() == false)
  {
    Export(false);
  }
}

void ForkAwareBatchSpanProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  is_shutdown_ = true;

  cv_.notify_one();
  worker_thread_.join();

  exporter_->Shutdown();
}

ForkAwareBatchSpanProcessor::~ForkAwareBatchSpanProcessor()
{
  if (is_shutdown_.load() == false)
  {
    Shutdown();
  }
}

/* FORK HANDLERS */

void ForkAwareBatchSpanProcessor::PrepareForFork() noexcept
{
  cv_m_.lock();
  force_flush_cv_m_.lock();
}

void ForkAwareBatchSpanProcessor::OnForkedParent() noexcept
{
  force_flush_cv_m_.unlock();
  cv_m_.unlock();
}

void ForkAwareBatchSpanProcessor::OnForkedChild() noexcept
{
  force_flush_cv_m_.unlock();
  cv_m_.unlock();
  // We don't want any spans to be duplicated so clear the buffer_
  // in the child
  // NOTE: We do not `Consume` any spans since they will anyway be consumed
  // in the parent process
  buffer_.Clear();

  // On forking, all other threads get eliminated. So we need to reclaim the resources
  // used by the eliminated threads, else they'll be zombies.
  worker_thread_.join();

  // We need to restart the background worker thread here too since
  // in a multithreaded environment, forking only duplicates the memory stack
  // of the thread that called it
  worker_thread_ = std::thread(&ForkAwareBatchSpanProcessor::DoBackgroundWork, this);
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
