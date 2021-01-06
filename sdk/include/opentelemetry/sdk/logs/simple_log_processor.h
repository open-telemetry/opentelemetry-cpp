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

#pragma once

#include <atomic>
#include <mutex>

#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * The simple log processor passes all log records
 * in a batch of 1 to the configured
 * LogExporter.
 *
 * All calls to the configured LogExporter are synchronized using a
 * spin-lock on an atomic_flag.
 */
class SimpleLogProcessor : public LogProcessor
{

public:
  explicit SimpleLogProcessor(std::unique_ptr<LogExporter> &&exporter);
  virtual ~SimpleLogProcessor() = default;

  std::unique_ptr<Recordable> MakeRecordable() noexcept override;

  void OnReceive(std::unique_ptr<Recordable> &&record) noexcept override;

  bool ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  // The configured exporter
  std::unique_ptr<LogExporter> exporter_;
  // The lock used to ensure the exporter is not called concurrently
  opentelemetry::common::SpinLockMutex lock_;
  // The atomic boolean flag to ensure the ShutDown() function is only called once
  std::atomic_flag shutdown_latch_ = ATOMIC_FLAG_INIT;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
