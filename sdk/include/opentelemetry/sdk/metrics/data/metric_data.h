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
/*struct MetricData
{
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  InstrumentDescriptor instrument_descriptor;
};*/

struct LongSingularMetricData
{
public:
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  InstrumentDescriptor instrument_descriptor;
  std::vector<std::pair<BasePointData, LongSingularPointData>> point_data_list;
};

struct DoubleSingularMetricData
{
public:
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  InstrumentDescriptor instrument_descriptor;
  std::vector<std::pair<BasePointData, DoubleSingularPointData>> point_data_list;
};

class LongHistogramMetricData
{
public:
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  InstrumentDescriptor instrument_descriptor;
  std::vector<std::pair<BasePointData, LongHistogramPointData>> point_datas_list;
};

class DoubleHistogramMetricData
{
public:
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  InstrumentDescriptor instrument_descriptor;
  std::vector<std::pair<BasePointData, DoubleHistogramPointData>> point_datas_list;
};

class DropMetricData
{};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif