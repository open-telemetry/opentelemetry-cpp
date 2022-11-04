// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <atomic>
#  include <mutex>

#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * The simple log processor passes all log records
 * in a batch of 1 to the configured
 * LogRecordExporter.
 *
 * All calls to the configured LogRecordExporter are synchronized using a
 * spin-lock on an atomic_flag.
 */
class SimpleLogProcessor : public LogRecordProcessor
{

public:
  explicit SimpleLogProcessor(std::unique_ptr<LogRecordExporter> &&exporter);
  ~SimpleLogProcessor() override = default;

  std::unique_ptr<Recordable> MakeRecordable() noexcept override;

  void OnEmit(std::unique_ptr<Recordable> &&record) noexcept override;

  bool ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  bool IsShutdown() const noexcept;

private:
  // The configured exporter
  std::unique_ptr<LogRecordExporter> exporter_;
  // The lock used to ensure the exporter is not called concurrently
  opentelemetry::common::SpinLockMutex lock_;
  // The atomic boolean to ensure the ShutDown() function is only called once
  std::atomic<bool> is_shutdown_;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
