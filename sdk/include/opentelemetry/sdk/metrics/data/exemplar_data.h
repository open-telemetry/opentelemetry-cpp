// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include <memory>
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
using MetricAttributes = opentelemetry::sdk::common::OrderedAttributeMap;
/**
 * A sample input measurement.
 *
 * Exemplars also hold information about the environment when the measurement was recorded, for
 * example the span and trace ID of the active span when the exemplar was recorded.
 */
class ExemplarData
{
public:
  static ExemplarData Create(std::shared_ptr<trace::SpanContext> context,
                             const opentelemetry::common::SystemTimestamp &timestamp,
                             const PointDataAttributes &point_data_attr)
  {
    return ExemplarData(context, timestamp, point_data_attr);
  }

  /**
   * The set of key/value pairs that were filtered out by the aggregator, but recorded alongside
   * the original measurement. Only key/value pairs that were filtered out by the aggregator
   * should be included
   */
  MetricAttributes GetFilteredAttributes() { return MetricAttributes{}; }

  /** Returns the timestamp in nanos when measurement was collected. */
  opentelemetry::common::SystemTimestamp GetEpochNanos() { return timestamp_; }

  /**
   * Returns the SpanContext associated with this exemplar. If the exemplar was not recorded
   * inside a sampled trace, the Context will be invalid.
   */
  std::shared_ptr<trace::SpanContext> GetSpanContext() { return context_.lock(); }

private:
  ExemplarData(std::shared_ptr<trace::SpanContext> context,
               opentelemetry::common::SystemTimestamp timestamp,
               const PointDataAttributes &point_data_attr)
      : context_(context), timestamp_(timestamp), point_data_attr_(point_data_attr)
  {}
  std::weak_ptr<trace::SpanContext> context_;
  opentelemetry::common::SystemTimestamp timestamp_;
  PointDataAttributes point_data_attr_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
