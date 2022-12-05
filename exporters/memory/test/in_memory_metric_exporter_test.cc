// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/memory/in_memory_metric_exporter.h"
#  include "google/protobuf/text_format.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"

#  include <gmock/gmock.h>
#  include <gtest/gtest.h>

#  include <iostream>

namespace metric_sdk     = opentelemetry::sdk::metrics;
namespace nostd          = opentelemetry::nostd;
namespace memoryexporter = opentelemetry::exporter::memory;

MATCHER_P(EqualsProto, pbtext, "")
{
  opentelemetry::proto::metrics::v1::ResourceMetrics p;
  if (!::google::protobuf::TextFormat::ParseFromString(pbtext, &p))
  {
    return false;
  }
  return p.DebugString() == arg->DebugString();
}

TEST(InMemoryMetricExporter, Shutdown)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new memoryexporter::InMemoryMetricExporter);

  ASSERT_TRUE(exporter->Shutdown());
  auto result = exporter->Export(metric_sdk::ResourceMetrics{});
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kFailure);
}

TEST(InMemoryMetricExporter, ExportSumPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new memoryexporter::InMemoryMetricExporter);

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
  auto attributes = metric_sdk::PointAttributes{{"a1", "b1"}};
  metric_sdk::MetricData metric_data{
      metric_sdk::InstrumentDescriptor{"library_name", "description", "unit",
                                       metric_sdk::InstrumentType::kCounter,
                                       metric_sdk::InstrumentValueType::kDouble},
      metric_sdk::AggregationTemporality::kDelta, opentelemetry::common::SystemTimestamp{},
      opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{{attributes, sum_point_data},
                                                   {attributes, sum_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);

  auto in_memory_exporter = static_cast<memoryexporter::InMemoryMetricExporter *>(exporter.get());
  auto in_memory_data     = in_memory_exporter->GetData().Get();
  EXPECT_EQ(in_memory_data.size(), 1);
  EXPECT_THAT(in_memory_data[0], EqualsProto(R"pb(
                resource {
                  attributes {
                    key: "service.name"
                    value { string_value: "unknown_service" }
                  }
                  attributes {
                    key: "telemetry.sdk.version"
                    value { string_value: "1.4.1" }
                  }
                  attributes {
                    key: "telemetry.sdk.name"
                    value { string_value: "opentelemetry" }
                  }
                  attributes {
                    key: "telemetry.sdk.language"
                    value { string_value: "cpp" }
                  }
                }
                instrumentation_library_metrics {
                  instrumentation_library { name: "library_name" version: "1.2.0" }
                  metrics {
                    name: "library_name"
                    description: "description"
                    unit: "unit"
                    sum {
                      data_points {
                        as_double: 10
                        attributes {
                          key: "a1"
                          value { string_value: "b1" }
                        }
                      }
                      data_points {
                        as_double: 20
                        attributes {
                          key: "a1"
                          value { string_value: "b1" }
                        }
                      }
                      aggregation_temporality: AGGREGATION_TEMPORALITY_DELTA
                      is_monotonic: true
                    }
                  }
                }
              )pb"));
}

TEST(InMemoryMetricExporter, ExportHistogramPointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new memoryexporter::InMemoryMetricExporter);

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
                                       metric_sdk::InstrumentType::kHistogram,
                                       metric_sdk::InstrumentValueType::kDouble},
      metric_sdk::AggregationTemporality::kDelta, opentelemetry::common::SystemTimestamp{},
      opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{
          {metric_sdk::PointAttributes{{"a1", "b1"}, {"a2", "b2"}}, histogram_point_data},
          {metric_sdk::PointAttributes{{"a1", "b1"}}, histogram_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);

  auto in_memory_exporter = static_cast<memoryexporter::InMemoryMetricExporter *>(exporter.get());
  auto in_memory_data     = in_memory_exporter->GetData().Get();
  EXPECT_EQ(in_memory_data.size(), 1);
  EXPECT_THAT(in_memory_data[0], EqualsProto(R"pb(
                resource {
                  attributes {
                    key: "service.name"
                    value { string_value: "unknown_service" }
                  }
                  attributes {
                    key: "telemetry.sdk.version"
                    value { string_value: "1.4.1" }
                  }
                  attributes {
                    key: "telemetry.sdk.name"
                    value { string_value: "opentelemetry" }
                  }
                  attributes {
                    key: "telemetry.sdk.language"
                    value { string_value: "cpp" }
                  }
                }
                instrumentation_library_metrics {
                  instrumentation_library { name: "library_name" version: "1.2.0" }
                  metrics {
                    name: "library_name"
                    description: "description"
                    unit: "unit"
                    histogram {
                      data_points {
                        count: 3
                        sum: 900.5
                        bucket_counts: 200
                        bucket_counts: 300
                        bucket_counts: 400
                        bucket_counts: 500
                        explicit_bounds: 10.1
                        explicit_bounds: 20.2
                        explicit_bounds: 30.2
                        attributes {
                          key: "a1"
                          value { string_value: "b1" }
                        }
                        attributes {
                          key: "a2"
                          value { string_value: "b2" }
                        }
                      }
                      data_points {
                        count: 3
                        sum: 900
                        bucket_counts: 200
                        bucket_counts: 300
                        bucket_counts: 400
                        bucket_counts: 500
                        explicit_bounds: 10
                        explicit_bounds: 20
                        explicit_bounds: 30
                        attributes {
                          key: "a1"
                          value { string_value: "b1" }
                        }
                      }
                      aggregation_temporality: AGGREGATION_TEMPORALITY_DELTA
                    }
                  }
                }
              )pb"));
}

TEST(InMemoryMetricExporter, ExportLastValuePointData)
{
  auto exporter =
      std::unique_ptr<metric_sdk::MetricExporter>(new memoryexporter::InMemoryMetricExporter);

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
                                       metric_sdk::InstrumentType::kObservableGauge,
                                       metric_sdk::InstrumentValueType::kDouble},
      metric_sdk::AggregationTemporality::kDelta, opentelemetry::common::SystemTimestamp{},
      opentelemetry::common::SystemTimestamp{},
      std::vector<metric_sdk::PointDataAttributes>{
          {metric_sdk::PointAttributes{}, last_value_point_data},
          {metric_sdk::PointAttributes{}, last_value_point_data2}}};
  data.instrumentation_info_metric_data_ = std::vector<metric_sdk::InstrumentationInfoMetrics>{
      {instrumentation_library.get(), std::vector<metric_sdk::MetricData>{metric_data}}};

  auto result = exporter->Export(data);
  EXPECT_EQ(result, opentelemetry::sdk::common::ExportResult::kSuccess);

  auto in_memory_exporter = static_cast<memoryexporter::InMemoryMetricExporter *>(exporter.get());
  auto in_memory_data     = in_memory_exporter->GetData().Get();
  EXPECT_EQ(in_memory_data.size(), 1);
  EXPECT_THAT(in_memory_data[0], EqualsProto(R"pb(
                resource {
                  attributes {
                    key: "service.name"
                    value { string_value: "unknown_service" }
                  }
                  attributes {
                    key: "telemetry.sdk.version"
                    value { string_value: "1.4.1" }
                  }
                  attributes {
                    key: "telemetry.sdk.name"
                    value { string_value: "opentelemetry" }
                  }
                  attributes {
                    key: "telemetry.sdk.language"
                    value { string_value: "cpp" }
                  }
                }
                instrumentation_library_metrics {
                  instrumentation_library { name: "library_name" version: "1.2.0" }
                  metrics {
                    name: "library_name"
                    description: "description"
                    unit: "unit"
                    gauge {
                      data_points { as_double: 10 }
                      data_points { as_int: 20 }
                    }
                  }
                }
              )pb"));
}

#endif
