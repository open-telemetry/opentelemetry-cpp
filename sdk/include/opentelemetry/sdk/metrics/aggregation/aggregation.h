// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Aggregation
{
public:
  virtual void Aggregate(long value, const PointAttributes &attributes = {}) noexcept = 0;

  virtual void Aggregate(double value, const PointAttributes &attributes = {}) noexcept = 0;

  /**
   * Returns the result of the merge of the two aggregations.
   *
   * This should always assume that the aggregations do not overlap and merge together for a new
   * cumulative report.
   *
   * @param delta the newly captured (delta) aggregation
   * @return the result of the merge of the given aggregation.
   */

  virtual std::unique_ptr<Aggregation> Merge(Aggregation &delta) noexcept = 0;

  /**
   * Returns a new delta aggregation by comparing two cumulative measurements.
   *
   * @param next the newly captured (cumulative) aggregation.
   * @return The resulting delta aggregation.
   */
  virtual std::unique_ptr<Aggregation> Diff(Aggregation &next) noexcept = 0;

  /**
   * Returns the point data that the aggregation will produce.
   *
   * @return PointType
   */

  virtual PointType ToPoint() noexcept = 0;

  virtual ~Aggregation() = default;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
