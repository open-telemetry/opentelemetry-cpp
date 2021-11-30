// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include "opentelemetry/sdk/common/exporter_utils.h"
#  include "opentelemetry/sdk/metrics/recordable.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * MetricExporter defines the interface to be used by metrics libraries to
 *  push metrics data to the OpenTelemetry exporters.
 */
class MetricExporter
{
public:
  virtual ~MetricExporter() = default;

  /**
   * Exports a batch of metrics recordables. This method must not be called
   * concurrently for the same exporter instance.
   * @param spans a span of unique pointers to metrics recordables
   */
  virtual opentelemetry::sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::metrics::Recordable>>
          &spans) noexcept = 0;

  /**
   * Force flush the exporter.
   */
  virtual bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept = 0;

  /**
   * Shut down the metric exporter.
   * @param timeout an optional timeout.
   * @return return the status of the operation.
   */
  virtual bool Shutdown() noexcept = 0;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
