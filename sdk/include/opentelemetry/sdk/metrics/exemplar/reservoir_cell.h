// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/metrics/data/exemplar_data.h"
#include "opentelemetry/sdk/metrics/exemplar/filter.h"
#include "opentelemetry/trace/context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * A Reservoir cell pre-allocated memories for Exemplar data.
 */
class ReservoirCell
{
public:
  ReservoirCell() = default;

  /**
   * Record the long measurement to the cell.
   */
  void RecordLongMeasurement(int64_t value,
                             const MetricAttributes &attributes,
                             const opentelemetry::context::Context &context)
  {
    value_ = value;
    offerMeasurement(attributes, context);
  }

  /**
   * Record the long measurement to the cell.
   */
  void RecordDoubleMeasurement(double value,
                               const MetricAttributes &attributes,
                               const opentelemetry::context::Context &context)
  {
    value_ = value;
    offerMeasurement(attributes, context);
  }

  /**
   * Retrieve the cell's {@link ExemplarData}.
   *
   * <p>Must be used in tandem with {@link #recordLongMeasurement(int64_t, Attributes, Context)}.
   */
  std::shared_ptr<ExemplarData> GetAndResetLong(const MetricAttributes &point_attributes)
  {
    if (!context_)
    {
      return nullptr;
    }
    auto attributes = attributes_;
    PointDataAttributes point_data_attributes;
    point_data_attributes.attributes = filtered(attributes, point_attributes);
    if (nostd::holds_alternative<int64_t>(value_))
    {
      point_data_attributes.point_data =
          ExemplarData::CreateSumPointData(nostd::get<int64_t>(value_));
    }
    std::shared_ptr<ExemplarData> result{
        new ExemplarData{ExemplarData::Create(context_, record_time_, point_data_attributes)}};
    reset();
    return result;
  }

  /**
   * Retrieve the cell's {@link ExemplarData}.
   *
   * <p>Must be used in tandem with {@link #recordDoubleMeasurement(double, Attributes, Context)}.
   */
  std::shared_ptr<ExemplarData> GetAndResetDouble(const MetricAttributes &point_attributes)
  {
    if (!context_)
    {
      return nullptr;
    }
    auto attributes = attributes_;
    PointDataAttributes point_data_attributes;
    point_data_attributes.attributes = filtered(attributes, point_attributes);
    if (nostd::holds_alternative<double>(value_))
    {
      point_data_attributes.point_data =
          ExemplarData::CreateSumPointData(nostd::get<double>(value_));
    }
    std::shared_ptr<ExemplarData> result{
        new ExemplarData{ExemplarData::Create(context_, record_time_, point_data_attributes)}};
    reset();
    return result;
  }

  void reset()
  {
    value_       = 0.0;
    record_time_ = opentelemetry::common::SystemTimestamp{};
  }

private:
  /** Returns filtered attributes for exemplars. */
  static MetricAttributes filtered(const MetricAttributes &original,
                                   const MetricAttributes &metric_point)
  {
    auto res = original;
    for (const auto &kv : metric_point)
    {
      auto it = res.find(kv.first);
      if (it != res.end())
      {
        res.erase(it);
      }
    }
    return res;
  }

  void offerMeasurement(const MetricAttributes &attributes,
                        const opentelemetry::context::Context &context)
  {
    attributes_  = attributes;
    record_time_ = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
    auto span    = opentelemetry::trace::GetSpan(context);
    if (span)
    {
      auto current_ctx = span->GetContext();
      if (current_ctx.IsValid())
      {
        context_.reset(new trace::SpanContext{current_ctx});
      }
    }
  }

  // Cell stores either long or double values, but must not store both
  std::shared_ptr<trace::SpanContext> context_;
  nostd::variant<int64_t, double> value_;
  opentelemetry::common::SystemTimestamp record_time_;
  MetricAttributes attributes_;
  // For testing
  friend class ReservoirCellTestPeer;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
