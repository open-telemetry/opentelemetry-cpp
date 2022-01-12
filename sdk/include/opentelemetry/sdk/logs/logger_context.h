// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/processor.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * A class which stores the LoggerContext context.
 *
 * This class meets the following design criteria:
 * - A shared reference between LoggerProvider and Logger instantiated.
 * - A thread-safe class that allows updating/altering processor/exporter pipelines
 *   and sampling config.
 * - The owner/destroyer of Processors/Exporters.  These will remain active until
 *   this class is destroyed.  I.e. Sampling, Exporting, flushing, Custom Iterator etc. are all ok
 * if this object is alive, and they will work together. If this object is destroyed, then no shared
 * references to Processor, Exporter, Recordable, Custom Iterator etc. should exist, and all
 *   associated pipelines will have been flushed.
 */
class LoggerContext
{
public:
  explicit LoggerContext(std::vector<std::unique_ptr<LogProcessor>> &&processors,
                         opentelemetry::sdk::resource::Resource resource =
                             opentelemetry::sdk::resource::Resource::Create({})) noexcept;

  /**
   * Attaches a log processor to list of configured processors to this tracer context.
   * Processor once attached can't be removed.
   * @param processor The new log processor for this tracer. This must not be
   * a nullptr. Ownership is given to the `TracerContext`.
   *
   * Note: This method is not thread safe.
   */
  void AddProcessor(std::unique_ptr<LogProcessor> processor) noexcept;

  /**
   * Obtain the configured (composite) processor.
   *
   * Note: When more than one processor is active, this will
   * return an "aggregate" processor
   */
  LogProcessor &GetProcessor() const noexcept;

  /**
   * Obtain the resource associated with this tracer context.
   * @return The resource for this tracer context.
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Force all active LogProcessors to flush any buffered logs
   * within the given timeout.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

  /**
   * Shutdown the log processor associated with this tracer provider.
   */
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept;

private:
  //  order of declaration is important here - resource object should be destroyed after processor.
  opentelemetry::sdk::resource::Resource resource_;
  std::unique_ptr<LogProcessor> processor_;
};
}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
