// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>
#  include <vector>

#  include "opentelemetry/sdk/logs/multi_recordable.h"
#  include "opentelemetry/sdk/logs/processor.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Log processor allow hooks for receive method invocations.
 *
 * Built-in log processors are responsible for batching and conversion of
 * logs to exportable representation and passing batches to exporters.
 */
class MultiLogProcessor : public LogProcessor
{
public:
  MultiLogProcessor(std::vector<std::unique_ptr<LogProcessor>> &&processors);
  ~MultiLogProcessor();

  void AddProcessor(std::unique_ptr<LogProcessor> &&processor);

  std::unique_ptr<Recordable> MakeRecordable() noexcept override;

  /**
   * OnReceive is called by the SDK once a log record has been successfully created.
   * @param record the log record
   */
  void OnReceive(std::unique_ptr<Recordable> &&record) noexcept override;

  /**
   * Exports all log records that have not yet been exported to the configured Exporter.
   * @param timeout that the forceflush is required to finish within.
   * @return a result code indicating whether it succeeded, failed or timed out
   */
  bool ForceFlush(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

  /**
   * Shuts down the processor and does any cleanup required.
   * ShutDown should only be called once for each processor.
   * @param timeout minimum amount of microseconds to wait for
   * shutdown before giving up and returning failure.
   * @return true if the shutdown succeeded, false otherwise
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override;

private:
  std::vector<std::unique_ptr<LogProcessor>> processors_;
};
}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
