// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <gtest/gtest.h>
#include <prometheus/metric_family.h>
#include <prometheus/text_serializer.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/resource/resource.h"

namespace metric_sdk = opentelemetry::sdk::metrics;
namespace nostd      = opentelemetry::nostd;

namespace prometheus_test
{

using opentelemetry::sdk::instrumentationscope::InstrumentationScope;
using opentelemetry::sdk::resource::Resource;
using opentelemetry::sdk::resource::ResourceAttributes;

struct TestDataPoints
{
  Resource resource{ResourceAttributes{{"service.name", "test_service"},
                                       {"service.namespace", "test_namespace"},
                                       {"service.instance.id", "localhost:8000"},
                                       {"custom_resource_attr", "custom_resource_value"}}};
  nostd::unique_ptr<InstrumentationScope> instrumentation_scope =
      InstrumentationScope::Create("library_name", "1.2.0");
  opentelemetry::common::SystemTimestamp start_ts =
      opentelemetry::common::SystemTimestamp{std::chrono::microseconds{1766662500000}};
  opentelemetry::common::SystemTimestamp end_ts =
      opentelemetry::common::SystemTimestamp{std::chrono::microseconds{1766662560000}};

  /**
   * Helper function to create ResourceMetrics
   */
  inline metric_sdk::ResourceMetrics CreateSumPointData()
  {
    metric_sdk::SumPointData sum_point_data{};
    sum_point_data.value_ = 10.0;
    metric_sdk::SumPointData sum_point_data2{};
    sum_point_data2.value_ = 20.0;
    metric_sdk::ResourceMetrics data;
    data.resource_ = &resource;
    metric_sdk::MetricData metric_data{
        metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                         metric_sdk::InstrumentType::kCounter,
                                         metric_sdk::InstrumentValueType::kDouble},
        metric_sdk::AggregationTemporality::kDelta, start_ts, end_ts,
        std::vector<metric_sdk::PointDataAttributes>{
            {metric_sdk::PointAttributes{{"a1", "b1"}}, sum_point_data},
            {metric_sdk::PointAttributes{{"a2", "b2"}}, sum_point_data2}}};
    data.scope_metric_data_ = std::vector<metric_sdk::ScopeMetrics>{
        {instrumentation_scope.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
    return data;
  }

  inline metric_sdk::ResourceMetrics CreateUpDownCounterPointData()
  {
    metric_sdk::SumPointData sum_point_data{};
    sum_point_data.value_        = -10.5;
    sum_point_data.is_monotonic_ = false;
    metric_sdk::SumPointData sum_point_data2{};
    sum_point_data2.value_        = 20.0;
    sum_point_data2.is_monotonic_ = false;
    metric_sdk::ResourceMetrics data;
    data.resource_ = &resource;
    metric_sdk::MetricData metric_data{
        metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                         metric_sdk::InstrumentType::kUpDownCounter,
                                         metric_sdk::InstrumentValueType::kDouble},
        metric_sdk::AggregationTemporality::kDelta, start_ts, end_ts,
        std::vector<metric_sdk::PointDataAttributes>{
            {metric_sdk::PointAttributes{{"a1", "b1"}}, sum_point_data},
            {metric_sdk::PointAttributes{{"a2", "b2"}}, sum_point_data2}}};
    data.scope_metric_data_ = std::vector<metric_sdk::ScopeMetrics>{
        {instrumentation_scope.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
    return data;
  }

  inline metric_sdk::ResourceMetrics CreateHistogramPointData()
  {
    metric_sdk::HistogramPointData histogram_point_data{};
    histogram_point_data.boundaries_ = {10.1, 20.2, 30.2};
    histogram_point_data.count_      = 3;
    histogram_point_data.counts_     = {200, 300, 400, 500};
    histogram_point_data.sum_        = 900.5;
    metric_sdk::HistogramPointData histogram_point_data2{};
    histogram_point_data2.boundaries_ = {10.0, 20.0, 30.0};
    histogram_point_data2.count_      = 3;
    histogram_point_data2.counts_     = {200, 300, 400, 500};
    histogram_point_data2.sum_        = static_cast<int64_t>(900);
    metric_sdk::ResourceMetrics data;
    data.resource_ = &resource;
    metric_sdk::MetricData metric_data{
        metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                         metric_sdk::InstrumentType::kHistogram,
                                         metric_sdk::InstrumentValueType::kDouble},
        metric_sdk::AggregationTemporality::kDelta, start_ts, end_ts,
        std::vector<metric_sdk::PointDataAttributes>{
            {metric_sdk::PointAttributes{{"a1", "b1"}}, histogram_point_data},
            {metric_sdk::PointAttributes{{"a2", "b2"}}, histogram_point_data2}}};
    data.scope_metric_data_ = std::vector<metric_sdk::ScopeMetrics>{
        {instrumentation_scope.get(), std::vector<metric_sdk::MetricData>{metric_data}}};

    return data;
  }

  inline metric_sdk::ResourceMetrics CreateLastValuePointData()
  {
    metric_sdk::ResourceMetrics data;
    data.resource_ = &resource;
    metric_sdk::LastValuePointData last_value_point_data{};
    last_value_point_data.value_              = 10.0;
    last_value_point_data.is_lastvalue_valid_ = true;
    last_value_point_data.sample_ts_          = opentelemetry::common::SystemTimestamp{};
    metric_sdk::LastValuePointData last_value_point_data2{};
    last_value_point_data2.value_              = static_cast<int64_t>(20);
    last_value_point_data2.is_lastvalue_valid_ = true;
    last_value_point_data2.sample_ts_          = opentelemetry::common::SystemTimestamp{};
    metric_sdk::MetricData metric_data{
        metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                         metric_sdk::InstrumentType::kCounter,
                                         metric_sdk::InstrumentValueType::kDouble},
        metric_sdk::AggregationTemporality::kDelta, start_ts, end_ts,
        std::vector<metric_sdk::PointDataAttributes>{
            {metric_sdk::PointAttributes{{"a1", "b1"}}, last_value_point_data},
            {metric_sdk::PointAttributes{{"a2", "b2"}}, last_value_point_data2}}};
    data.scope_metric_data_ = std::vector<metric_sdk::ScopeMetrics>{
        {instrumentation_scope.get(), std::vector<metric_sdk::MetricData>{metric_data}}};
    return data;
  }
};

inline std::string SerializeMetrics(const std::vector<::prometheus::MetricFamily> &metrics)
{
  ::prometheus::TextSerializer serializer;
  return serializer.Serialize(metrics);
}

inline std::string ReadTestDataFile(const std::string &filename)
{
  const std::vector<std::string> candidate_paths = {
      "exporters/prometheus/test/testdata/" + filename,
      "testdata/" + filename,
      "../exporters/prometheus/test/testdata/" + filename,
  };
  for (const auto &path : candidate_paths)
  {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (file.is_open())
    {
      std::ostringstream ss;
      ss << file.rdbuf();
      std::string content = ss.str();
      content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
      return content;
    }
  }
  ADD_FAILURE() << "Failed to open testdata file: " << filename;
  return "";
}
}  // namespace prometheus_test
