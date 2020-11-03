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
#include "opentelemetry/logs/log_record.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
enum class ShutdownResult
{
  kSuccess = 0,
  kFailure = 1,
  kTimeout = 2
};
/**
 * This Log Processor is responsible for the batching of log records
 * and passing them to exporters.
 */
class LogProcessor
{
public:
  virtual ~LogProcessor() = default;

  /**
   * OnReceive is responsible for batching every log record that is created by the SDK
   * @param record a log record that has all the user data and injected data
   */
  virtual void OnReceive(std::unique_ptr<opentelemetry::logs::LogRecord> &&record) noexcept = 0;

  /**
   * Exports all log records that have not yet been exported to the configured Exporter.
   * @param timeout that the forceflush is required to finish within.
   * A default timeout of 0 mean no timeout is applied.
   * @return a result code indicating whether it succeeded, failed or timed out
   */
  virtual ShutdownResult ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept = 0;

  /**
   * Shuts down the processor and does any cleanup required.
   * ShutDown should only be called once for each processor.
   * @param timeout that the shutdown should finish within.
   * A default timeout of 0 means no timeout is applied.
   * @return a ShutDown result (if it succeeded, failed or timed out)
   */
  virtual ShutdownResult Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept = 0;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
