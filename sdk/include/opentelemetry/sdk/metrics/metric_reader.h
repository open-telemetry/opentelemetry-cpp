// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <chrono>
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * MetricReader defines the interface to collect metrics from SDK
 */
class MetricReader
{
public:
  virtual ~MetricReader() = default;

  /**
   * Collect the metrics from SDK.
   * @return return the status of the operation.
   */
  virtual bool Collect() noexcept = 0;

  /**
   * Shut down the metric reader.
   * @param timeout an optional timeout.
   * @return return the status of the operation.
   */
  virtual bool Shutdown() noexcept = 0;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
