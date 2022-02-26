// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/* Represent the storage from which to collect the metrics */

class MetricStorage
{
public:
  /* collect the metrics from this storage */
  virtual bool Collect(
      MetricCollector *collector,
      nostd::span<MetricCollector *> collectors,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::resource::Resource *resource,
      nostd::function_ref<bool(MetricData &)> callback) noexcept = 0;
};

class WritableMetricStorage
{
public:
  virtual void RecordLong(long value) noexcept = 0;

  virtual void RecordLong(long value,
                          const opentelemetry::common::KeyValueIterable &attributes) noexcept = 0;

  virtual void RecordDouble(double value) noexcept = 0;

  virtual void RecordDouble(double value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept = 0;
};

class NoopMetricStorage : public MetricStorage
{
public:
  bool Collect(
      MetricCollector *collector,
      nostd::span<MetricCollector *> collectors,
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
      opentelemetry::sdk::resource::Resource *resource,
      nostd::function_ref<bool(MetricData &)> callback) noexcept override
  {
    MetricData metric_data;
    if (callback(metric_data))
    {
      return true;
    }
    return false;
  }
};

class NoopWritableMetricStorage : public WritableMetricStorage
{
public:
  void RecordLong(long value) noexcept = 0;

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {}

  void RecordDouble(double value) noexcept override {}

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {}
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
