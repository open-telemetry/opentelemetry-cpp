// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW
#  include "opentelemetry/sdk/logs/simple_log_processor.h"

#  include <chrono>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * Initialize a simple log processor.
 * @param exporter the configured exporter where log records are sent
 */
SimpleLogProcessor::SimpleLogProcessor(std::unique_ptr<LogExporter> &&exporter,
                                       bool is_export_async)
    : exporter_(std::move(exporter))
#  ifdef ENABLE_ASYNC_EXPORT
      ,
      is_export_async_(is_export_async)
#  else
      ,
      is_export_async_(false)
#  endif
{}

std::unique_ptr<Recordable> SimpleLogProcessor::MakeRecordable() noexcept
{
  return exporter_->MakeRecordable();
}

/**
 * Batches the log record it receives in a batch of 1 and immediately sends it
 * to the configured exporter
 */
void SimpleLogProcessor::OnReceive(std::unique_ptr<Recordable> &&record) noexcept
{
  nostd::span<std::unique_ptr<Recordable>> batch(&record, 1);
  // Get lock to ensure Export() is never called concurrently
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);

  if (is_export_async_ == false)
  {
    if (exporter_->Export(batch) != sdk::common::ExportResult::kSuccess)
    {
      /* Alert user of the failed export */
    }
  }
#  ifdef ENABLE_ASYNC_EXPORT
  else
  {
    exporter_->Export(batch, [](sdk::common::ExportResult result) {
      /* Log the result
       */
      return true;
    });
  }
#  endif
}
/**
 *  The simple processor does not have any log records to flush so this method is not used
 */
bool SimpleLogProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return true;
}

bool SimpleLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  // Should only shutdown exporter ONCE.
  if (!shutdown_latch_.test_and_set(std::memory_order_acquire) && exporter_ != nullptr)
  {
    return exporter_->Shutdown(timeout);
  }

  return true;
}
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
