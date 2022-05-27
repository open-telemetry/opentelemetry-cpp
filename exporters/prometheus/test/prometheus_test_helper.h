// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/version.h"

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace exportermetrics = opentelemetry::exporter::metrics;

/**
 * Helper function to create ResourceMetrics
 */
metric_sdk::ResourceMetrics CreateSumPointData()
{
  metric_sdk::SumPointData sum_point_data{};
  sum_point_data.value_ = 10.0;
  metric_sdk::SumPointData sum_point_data2{};
  sum_point_data2.value_ = 20.0;
  metric_sdk::ResourceMetrics data;
  auto resource = opentelemetry::sdk::resource::Resource::Create(
      opentelemetry::sdk::resource::ResourceAttributes{});
  data.resource_ = &resource;
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{
          {metric_sdk::PointAttributes{}, sum_point_data},
          {metric_sdk::PointAttributes{}, sum_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
  return data;
}

metric_sdk::ResourceMetrics CreateHistogramPointData()
{
  metric_sdk::HistogramPointData histogram_point_data{};
  histogram_point_data.boundaries_ = std::list<double>{10.1, 20.2, 30.2};
  histogram_point_data.count_      = 3;
  histogram_point_data.counts_     = {200, 300, 400, 500};
  histogram_point_data.sum_        = 900.5;
  metric_sdk::HistogramPointData histogram_point_data2{};
  histogram_point_data2.boundaries_ = std::list<long>{10, 20, 30};
  histogram_point_data2.count_      = 3;
  histogram_point_data2.counts_     = {200, 300, 400, 500};
  histogram_point_data2.sum_        = 900l;
  metric_sdk::ResourceMetrics data;
  auto resource = opentelemetry::sdk::resource::Resource::Create(
      opentelemetry::sdk::resource::ResourceAttributes{});
  data.resource_ = &resource;
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{
          {metric_sdk::PointAttributes{}, histogram_point_data},
          {metric_sdk::PointAttributes{}, histogram_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
  return data;
}

metric_sdk::ResourceMetrics CreateLastValuePointData()
{
  metric_sdk::ResourceMetrics data;
  auto resource = opentelemetry::sdk::resource::Resource::Create(
      opentelemetry::sdk::resource::ResourceAttributes{});
  data.resource_ = &resource;
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  metric_sdk::LastValuePointData last_value_point_data{};
  last_value_point_data.value_              = 10.0;
  last_value_point_data.is_lastvalue_valid_ = true;
  last_value_point_data.sample_ts_          = opentelemetry::common::SystemTimestamp{};
  metric_sdk::LastValuePointData last_value_point_data2{};
  last_value_point_data2.value_              = 20l;
  last_value_point_data2.is_lastvalue_valid_ = true;
  last_value_point_data2.sample_ts_          = opentelemetry::common::SystemTimestamp{};
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{
          {metric_sdk::PointAttributes{}, last_value_point_data},
          {metric_sdk::PointAttributes{}, last_value_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
  return data;
}

metric_sdk::ResourceMetrics CreateDropPointData()
{
  metric_sdk::ResourceMetrics data;
  auto resource = opentelemetry::sdk::resource::Resource::Create(
      opentelemetry::sdk::resource::ResourceAttributes{});
  data.resource_ = &resource;
  auto instrumentation_library =
      opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary::Create("library_name",
                                                                                 "1.2.0");
  metric_sdk::DropPointData drop_point_data{};
  metric_sdk::DropPointData drop_point_data2{};
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      opentelemetry::common::SystemTimestamp{}, opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{
          {metric_sdk::PointAttributes{}, drop_point_data},
          {metric_sdk::PointAttributes{}, drop_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
  return data;
}

#endif  // ENABLE_METRICS_PREVIEW
