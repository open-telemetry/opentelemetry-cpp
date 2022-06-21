// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#ifndef ENABLE_METRICS_PREVIEW

#  include <gtest/gtest.h>
#  include <memory>
#  include <vector>
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/resource/resource_detector.h"

#  include <iostream>
#  include "opentelemetry/exporters/ostream/metric_exporter.h"
#  include "opentelemetry/sdk/metrics/aggregation/default_aggregation.h"
#  include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/resource/resource.h"

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace exportermetrics = opentelemetry::exporter::metrics;

TEST(OStreamMetricsExporter, Shutdown)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);
  ASSERT_TRUE(exporter->Shutdown());
  auto result = exporter->Export(metric_sdk::ResourceMetrics{});
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

TEST(OStreamMetricsExporter, ExportSumPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

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
          {metric_sdk::PointAttributes{{"a1", "b1"}}, sum_point_data},
          {metric_sdk::PointAttributes{{"a1", "b1"}}, sum_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expected_output =
      "{"
      "\n  name\t\t: library_name"
      "\n  schema url\t: "
      "\n  version\t: 1.2.0"
      "\n  start time\t: Thu Jan  1 00:00:00 1970"
      "\n  end time\t: Thu Jan  1 00:00:00 1970"
      "\n  name\t\t: library_name"
      "\n  description\t: description"
      "\n  unit\t\t: unit"
      "\n  type\t\t: SumPointData"
      "\n  value\t\t: 10"
      "\n  attributes\t\t: "
      "\n\ta1: b1"
      "\n  type\t\t: SumPointData"
      "\n  value\t\t: 20"
      "\n  attributes\t\t: "
      "\n\ta1: b1"
      "\n}\n";
  ASSERT_EQ(stdoutOutput.str(), expected_output);
}

TEST(OStreamMetricsExporter, ExportHistogramPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

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
          {metric_sdk::PointAttributes{{"a1", "b1"}, {"a2", "b2"}}, histogram_point_data},
          {metric_sdk::PointAttributes{{"a1", "b1"}}, histogram_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expected_output =
      "{"
      "\n  name\t\t: library_name"
      "\n  schema url\t: "
      "\n  version\t: 1.2.0"
      "\n  start time\t: Thu Jan  1 00:00:00 1970"
      "\n  end time\t: Thu Jan  1 00:00:00 1970"
      "\n  name\t\t: library_name"
      "\n  description\t: description"
      "\n  unit\t\t: unit"
      "\n  type     : HistogramPointData"
      "\n  count     : 3"
      "\n  sum     : 900.5"
      "\n  buckets     : [10.1, 20.2, 30.2, ]"
      "\n  counts     : [200, 300, 400, 500, ]"
      "\n  attributes\t\t: "
      "\n\ta1: b1"
      "\n\ta2: b2"
      "\n  type     : HistogramPointData"
      "\n  count     : 3"
      "\n  sum     : 900"
      "\n  buckets     : [10, 20, 30, ]"
      "\n  counts     : [200, 300, 400, 500, ]"
      "\n  attributes\t\t: "
      "\n\ta1: b1"
      "\n}\n";
  ASSERT_EQ(stdoutOutput.str(), expected_output);
}

TEST(OStreamMetricsExporter, ExportLastValuePointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

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

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expected_output =
      "{"
      "\n  name\t\t: library_name"
      "\n  schema url\t: "
      "\n  version\t: 1.2.0"
      "\n  start time\t: Thu Jan  1 00:00:00 1970"
      "\n  end time\t: Thu Jan  1 00:00:00 1970"
      "\n  name\t\t: library_name"
      "\n  description\t: description"
      "\n  unit\t\t: unit"
      "\n  type     : LastValuePointData"
      "\n  timestamp     : 0"
      "\n  valid     : true"
      "\n  value     : 10"
      "\n  attributes\t\t: "
      "\n  type     : LastValuePointData"
      "\n  timestamp     : 0"
      "\n  valid     : true"
      "\n  value     : 20"
      "\n  attributes\t\t: "
      "\n}\n";
  ASSERT_EQ(stdoutOutput.str(), expected_output);
}

TEST(OStreamMetricsExporter, ExportDropPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new exportermetrics::OStreamMetricExporter);

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

  std::stringstream stdoutOutput;
  std::streambuf *sbuf = std::cout.rdbuf();
  std::cout.rdbuf(stdoutOutput.rdbuf());

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);
  std::cout.rdbuf(sbuf);

  std::string expected_output =
      "{"
      "\n  name\t\t: library_name"
      "\n  schema url\t: "
      "\n  version\t: 1.2.0"
      "\n  start time\t: Thu Jan  1 00:00:00 1970"
      "\n  end time\t: Thu Jan  1 00:00:00 1970"
      "\n  name\t\t: library_name"
      "\n  description\t: description"
      "\n  unit\t\t: unit"
      "\n}\n";

  ASSERT_EQ(stdoutOutput.str(), expected_output);
}

#endif
