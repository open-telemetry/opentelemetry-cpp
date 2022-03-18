// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/**
 * MetricProducer is the interface that is used to make metric data available to the
 * OpenTelemetry exporters. Implementations should be stateful, in that each call to
 * `Collect` will return any metric generated since the last call was made.
 *
 * <p>Implementations must be thread-safe.
 */

class MetricProducer
{
public:
  /**
   * The callback to be called for each metric exporter. This will only be those
   * metrics that have been produced since the last time this method was called.
   *
   * @return a status of completion of method.
   */
  virtual bool Collect(nostd::function_ref<bool(MetricData)> callback) noexcept = 0;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
