// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/context/context.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/* Represent the storage from which to collect the metrics */
class CollectorHandle;

class MetricStorage
{
public:
  /* collect the metrics from this storage */
  virtual bool Collect(CollectorHandle *collector,
                       nostd::span<std::shared_ptr<CollectorHandle>> collectors,
                       opentelemetry::common::SystemTimestamp sdk_start_ts,
                       opentelemetry::common::SystemTimestamp collection_ts,
                       nostd::function_ref<bool(MetricData)> callback) noexcept = 0;
};

class WritableMetricStorage
{
public:
  virtual void RecordLong(long value, const opentelemetry::context::Context &context) noexcept = 0;

  virtual void RecordLong(long value,
                          const opentelemetry::common::KeyValueIterable &attributes,
                          const opentelemetry::context::Context &context) noexcept = 0;

  virtual void RecordDouble(double value,
                            const opentelemetry::context::Context &context) noexcept = 0;

  virtual void RecordDouble(double value,
                            const opentelemetry::common::KeyValueIterable &attributes,
                            const opentelemetry::context::Context &context) noexcept = 0;

  virtual ~WritableMetricStorage() = default;
};

class NoopMetricStorage : public MetricStorage
{
public:
  bool Collect(CollectorHandle *collector,
               nostd::span<std::shared_ptr<CollectorHandle>> collectors,
               opentelemetry::common::SystemTimestamp sdk_start_ts,
               opentelemetry::common::SystemTimestamp collection_ts,
               nostd::function_ref<bool(MetricData)> callback) noexcept override
  {
    MetricData metric_data;
    return callback(std::move(metric_data));
  }
};

class NoopWritableMetricStorage : public WritableMetricStorage
{
public:
  void RecordLong(long value, const opentelemetry::context::Context &context) noexcept = 0;

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes,
                  const opentelemetry::context::Context &context) noexcept override
  {}

  void RecordDouble(double value, const opentelemetry::context::Context &context) noexcept override
  {}

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes,
                    const opentelemetry::context::Context &context) noexcept override
  {}
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
