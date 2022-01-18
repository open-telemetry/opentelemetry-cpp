// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

#  include <vector>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
template <class T>
struct MetricData
{
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  InstrumentDescriptor instrument_descriptor;
};

template <class T>
struct SingularMetricsData : public MetricData<T>
{
public:
  std::vector<std::pair<BasePointData, SingularPointData<T>>> point_data_list;
};

template <class T>
class HistogramMetricData : public MetricData<T>
{
public:
  std::vector<std::pair<BasePointData, HistogramPointData<T>>> point_datas_list;
};

template <class T>
class DropMetricData : public MetricData<T>
{};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif