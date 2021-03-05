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

#include <chrono>
#include <memory>
#include "opentelemetry/sdk/logs/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * The Log Processor is responsible for passing log records
 * to the configured exporter.
 */
class LogProcessor
{
public:
  virtual ~LogProcessor() = default;

  /**
   * Create a log recordable. This requests a new log recordable from the
   * associated exporter.
   * @return a newly initialized recordable
   *
   * Note: This method must be callable from multiple threads.
   */
  virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

  /**
   * OnReceive is called by the SDK once a log record has been successfully created.
   * @param record the log record
   */
  virtual void OnReceive(std::unique_ptr<Recordable> &&record) noexcept = 0;

  /**
   * Exports all log records that have not yet been exported to the configured Exporter.
   * @param timeout that the forceflush is required to finish within.
   * @return a result code indicating whether it succeeded, failed or timed out
   */
  virtual bool ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept = 0;

  /**
   * Shuts down the processor and does any cleanup required.
   * ShutDown should only be called once for each processor.
   * @param timeout minimum amount of microseconds to wait for
   * shutdown before giving up and returning failure.
   * @return true if the shutdown succeeded, false otherwise
   */
  virtual bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept = 0;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
