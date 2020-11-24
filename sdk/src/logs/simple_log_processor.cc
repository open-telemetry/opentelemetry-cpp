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

#include "opentelemetry/sdk/logs/simple_log_processor.h"

#include <chrono>
#include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * Initialize a simple log processor.
 * @param exporter the configured exporter where log records are sent
 */
SimpleLogProcessor::SimpleLogProcessor(std::unique_ptr<LogExporter> &&exporter)
    : exporter_(std::move(exporter))
{}

/**
 * Batches the log record it receives in a batch of 1 and immediately sends it
 * to the configured exporter
 */
void SimpleLogProcessor::OnReceive(
    std::unique_ptr<opentelemetry::logs::LogRecord> &&record) noexcept
{
  std::vector<std::unique_ptr<opentelemetry::logs::LogRecord>> batch;
  batch.emplace_back(std::move(record));

  // Get lock to ensure Export() is never called concurrently
  std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);

  if (exporter_->Export(batch) != ExportResult::kSuccess)
  {
    /* TODO: alert user of the failed or timedout export result */
  }
}
/**
 *  The simple processor does not have any log records to flush so this method is not used
 */
bool SimpleLogProcessor::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return true;
}

/**
 * TODO: This method should not block indefinitely. Should abort within timeout.
 */
bool SimpleLogProcessor::Shutdown(std::chrono::microseconds timeout) noexcept
{
  if (timeout < std::chrono::microseconds(0))
  {
    // TODO: alert caller of invalid timeout?
    return false;
  }

  // Should only shutdown exporter ONCE.
  if (!shutdown_latch_.test_and_set(std::memory_order_acquire))
  {
    return exporter_->Shutdown(timeout);
  }

  return false;
}
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
