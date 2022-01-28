// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable_view.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"
#  include "opentelemetry/sdk/resource/resource.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class SyncMetricStorage : public MetricStorage, public WritableMetricStorage
{

public:
  SyncMetricStorage(
      // TBD
      // Aggregation& aggregation
      // opentelemetry::sdk::resource::resource *resource,
      // sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
  )
  {}

  void RecordLong(long value) noexcept override
  {
    // TBD
    // aggregation->recordLong(value);
  }

  void RecordLong(long value,
                  const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    // TBD
    // aggregation->recordLong(value, attributes);
  }

  void RecordDouble(double value) noexcept override
  {
    // TBD
    // aggregation->recordDouble(value, attributes);
  }

  void RecordDouble(double value,
                    const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    // TBD
    // aggregation->recordDouble(value, attributes);
  }

  bool Collect(AggregationTemporarily aggregation_temporarily,
               nostd::function_ref<bool(MetricData)> callback) noexcept override
  {
    if (callback(MetricData()))
    {
      return true;
    }
    return false;
  }
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
