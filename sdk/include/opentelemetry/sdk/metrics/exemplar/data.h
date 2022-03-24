// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"

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
  /**
   * The set of key/value pairs that were filtered out by the aggregator, but recorded alongside the
   * original measurement. Only key/value pairs that were filtered out by the aggregator should be
   * included
   */
  MetricAttributes GetFilteredAttributes();

  /** Returns the timestamp in nanos when measurement was collected. */
  opentelemetry::common::SystemTimestamp GetEpochNanos();

  /**
   * Returns the SpanContext associated with this exemplar. If the exemplar was not recorded
   * inside a sampled trace, the Context will be invalid.
   */
  opentelemetry::context::Context GetSpanContext();
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
