// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stdint.h>
#include <chrono>
#include <memory>

#include "opentelemetry/context/context.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace instrumentationscope
{
class InstrumentationScope;
}  // namespace instrumentationscope
}  // namespace sdk

namespace sdk
{
namespace logs
{
class Recordable;

/**
 * The Log Processor is responsible for passing log records
 * to the configured exporter.
 */
class LogRecordProcessor
{
public:
  LogRecordProcessor() = default;

  LogRecordProcessor(const LogRecordProcessor &)            = delete;
  LogRecordProcessor(LogRecordProcessor &&)                 = delete;
  LogRecordProcessor &operator=(const LogRecordProcessor &) = delete;
  LogRecordProcessor &operator=(LogRecordProcessor &&)      = delete;

  virtual ~LogRecordProcessor() = default;

  /**
   * Create a log recordable. This requests a new log recordable from the
   * associated exporter.
   *
   * @return a newly initialized recordable
   *
   * Note: This method must be callable from multiple threads.
   */
  virtual std::unique_ptr<Recordable> MakeRecordable() noexcept = 0;

  /**
   * OnEmit is called by the SDK once a log record has been successfully created.
   * @param record the log recordable object
   */
  virtual void OnEmit(std::unique_ptr<Recordable> &&record) noexcept = 0;

  /**
   * Enabled returns whether this processor is interested in a log with the given severity.
   * Default minimum is permissive (every severity passes); concrete processors call
   * SetMinimumSeverity in their constructor when they want severity filtering.
   */
  inline bool Enabled(opentelemetry::logs::Severity severity) const noexcept
  {
    return static_cast<uint8_t>(severity) >= OPENTELEMETRY_ATOMIC_READ_8(&minimum_severity_);
  }

  /**
   * Enabled returns whether this processor is interested in a log with the given inputs.
   */
  bool Enabled(
      const opentelemetry::context::Context &context,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope &instrumentation_scope,
      opentelemetry::logs::Severity severity,
      opentelemetry::nostd::string_view event_name = {}) const noexcept
  {
    if (!Enabled(severity))
    {
      return false;
    }
    return EnabledImplementation(context, instrumentation_scope, severity, event_name);
  }

  /**
   * Exports all log records that have not yet been exported to the configured Exporter.
   * @param timeout that the forceflush is required to finish within.
   * @return a result code indicating whether it succeeded, failed or timed out
   */
  virtual bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept = 0;

  /**
   * Shuts down the processor and does any cleanup required.
   * ShutDown should only be called once for each processor.
   * @param timeout minimum amount of microseconds to wait for
   * shutdown before giving up and returning failure.
   * @return true if the shutdown succeeded, false otherwise
   */
  virtual bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept = 0;

protected:
  /**
   * SetMinimumSeverity sets the minimum severity at or above which this
   * processor reports as enabled via the severity-only @ref Enabled overload.
   * Concrete processors call this from their constructor to declare a severity
   * floor; the cached value is read without virtual dispatch.
   *
   * @param severity_or_max the minimum severity, cast from
   *   opentelemetry::logs::Severity. Pass opentelemetry::logs::kMaxSeverity to
   *   disable every severity. The default (when this method is not called) is
   *   0, meaning every severity passes.
   *
   * @note The write is atomic; safe to call concurrently with @ref Enabled
   *   reads on other threads.
   */
  void SetMinimumSeverity(uint8_t severity_or_max) noexcept
  {
    OPENTELEMETRY_ATOMIC_WRITE_8(&minimum_severity_, severity_or_max);
  }

  virtual bool EnabledImplementation(
      const opentelemetry::context::Context & /*context*/,
      const opentelemetry::sdk::instrumentationscope::InstrumentationScope
          & /*instrumentation_scope*/,
      opentelemetry::logs::Severity /*severity*/,
      opentelemetry::nostd::string_view /*event_name*/) const noexcept
  {
    return true;
  }

private:
  //
  // minimum_severity_ can be updated concurrently by multiple threads/cores, so race conditions
  // on read/write are handled with atomic operations. Defaults to 0 (kInvalid) so every severity
  // passes by default, matching the prior permissive behavior.
  //
  mutable uint8_t minimum_severity_{0};
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
