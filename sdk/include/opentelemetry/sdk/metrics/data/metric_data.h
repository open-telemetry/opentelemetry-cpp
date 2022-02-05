// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using PointAttributes = opentelemetry::sdk::common::AttributeMap;
using PointType       = opentelemetry::nostd::
    variant<SumPointData, HistogramPointData, LastValuePointData, DropPointData>;

class MetricData
{
public:
  opentelemetry::sdk::resource::Resource *resource_;
  opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  PointAttributes attributes_;
  InstrumentDescriptor instrument_descriptor;
  PointType point_data_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif