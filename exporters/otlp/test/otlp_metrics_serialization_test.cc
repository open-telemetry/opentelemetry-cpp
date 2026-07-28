// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/exporters/otlp/otlp_metric_utils.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/metrics/data/circular_buffer.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include <google/protobuf/repeated_ptr_field.h>
#include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/proto/metrics/v1/metrics.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace resource      = opentelemetry::sdk::resource;
namespace proto         = opentelemetry::proto;
namespace metrics_sdk   = opentelemetry::sdk::metrics;
namespace otlp_exporter = opentelemetry::exporter::otlp;

static metrics_sdk::MetricData CreateSumAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {"Counter", "desc", "unit",
                                                 metrics_sdk::InstrumentType::kCounter,
                                                 metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::SumPointData s_data_1, s_data_2;
  s_data_1.value_ = 10.2;
  s_data_2.value_ = 20.2;

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"k1", "v1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"k2", "v2"}};
  point_data_attr_2.point_data = s_data_2;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

static metrics_sdk::MetricData CreateUpDownCounterAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {"UpDownCounter", "Robot Pose Y", "Meter",
                                                 metrics_sdk::InstrumentType::kUpDownCounter,
                                                 metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::SumPointData s_data_1, s_data_2;
  s_data_1.value_ = 1.35;
  s_data_2.value_ = 1.37;

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"environment_id", "DEV-AYS"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"robot_id", "DEV-AYS-03-02"}};
  point_data_attr_2.point_data = s_data_2;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

static metrics_sdk::MetricData CreateHistogramAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {"Histogram", "desc", "unit",
                                                 metrics_sdk::InstrumentType::kCounter,
                                                 metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::HistogramPointData s_data_1, s_data_2;
  s_data_1.sum_        = 100.2;
  s_data_1.count_      = 22;
  s_data_1.counts_     = {2, 9, 4, 7};
  s_data_1.boundaries_ = std::vector<double>({0.0, 10.0, 20.0, 30.0});
  s_data_2.sum_        = 200.2;
  s_data_2.count_      = 20;
  s_data_2.counts_     = {0, 8, 5, 7};
  s_data_2.boundaries_ = std::vector<double>({0.0, 10.0, 20.0, 30.0});

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"k1", "v1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"k2", "v2"}};
  point_data_attr_2.point_data = s_data_2;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

static metrics_sdk::MetricData CreateExponentialHistogramAggregationData(
    const std::chrono::system_clock::time_point &now_time)
{
  metrics_sdk::MetricData data;
  data.start_ts                               = opentelemetry::common::SystemTimestamp(now_time);
  metrics_sdk::InstrumentDescriptor inst_desc = {"Histogram", "desc", "unit",
                                                 metrics_sdk::InstrumentType::kHistogram,
                                                 metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::Base2ExponentialHistogramPointData s_data_1, s_data_2;
  s_data_1.count_          = 3;
  s_data_1.sum_            = 6.5;
  s_data_1.min_            = 0.0;
  s_data_1.max_            = 3.5;
  s_data_1.scale_          = 3;
  s_data_1.record_min_max_ = true;
  s_data_1.zero_count_     = 1;
  s_data_1.positive_buckets_ =
      std::make_unique<opentelemetry::sdk::metrics::AdaptingCircularBufferCounter>(10);
  s_data_1.negative_buckets_ =
      std::make_unique<opentelemetry::sdk::metrics::AdaptingCircularBufferCounter>(10);
  s_data_1.positive_buckets_->Increment(1, 1);
  s_data_1.negative_buckets_->Increment(-2, 1);

  s_data_2.count_          = 4;
  s_data_2.sum_            = 6.2;
  s_data_2.min_            = -0.03;
  s_data_2.max_            = 3.5;
  s_data_2.scale_          = 3;
  s_data_2.record_min_max_ = false;
  s_data_2.zero_count_     = 2;
  s_data_2.positive_buckets_ =
      std::make_unique<opentelemetry::sdk::metrics::AdaptingCircularBufferCounter>(10);
  s_data_2.negative_buckets_ =
      std::make_unique<opentelemetry::sdk::metrics::AdaptingCircularBufferCounter>(10);
  s_data_2.positive_buckets_->Increment(3, 1);
  s_data_2.negative_buckets_->Increment(-2, 1);
  s_data_2.negative_buckets_->Increment(-4, 2);

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts                  = opentelemetry::common::SystemTimestamp(now_time);
  data.instrument_descriptor   = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"k1", "v1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"k2", "v2"}};
  point_data_attr_2.point_data = s_data_2;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

static metrics_sdk::MetricData CreateObservableGaugeAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {"LastValue", "desc", "unit",
                                                 metrics_sdk::InstrumentType::kObservableGauge,
                                                 metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::LastValuePointData s_data_1, s_data_2;
  s_data_1.value_ = 30.2;
  s_data_2.value_ = 50.2;

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"k1", "v1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"k2", "v2"}};
  point_data_attr_2.point_data = s_data_2;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

static metrics_sdk::MetricData CreateObservableCounterAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {
      "ObservableCounter", "test description", "test unit",
      metrics_sdk::InstrumentType::kObservableCounter, metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::SumPointData s_data_1, s_data_2;
  s_data_1.value_ = 1.23;
  s_data_2.value_ = 4.56;

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"key1", "value1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"key2", "value2"}};
  point_data_attr_2.point_data = s_data_2;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

static metrics_sdk::MetricData CreateObservableUpDownCounterAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {
      "ObservableUpDownCounter", "test description", "test unit",
      metrics_sdk::InstrumentType::kObservableUpDownCounter,
      metrics_sdk::InstrumentValueType::kDouble};
  metrics_sdk::SumPointData s_data_1, s_data_2, s_data_3;
  s_data_1.value_ = 1.23;
  s_data_2.value_ = 4.56;
  s_data_3.value_ = 2.34;

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2, point_data_attr_3;
  point_data_attr_1.attributes = {{"key1", "value1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"key2", "value2"}};
  point_data_attr_2.point_data = s_data_2;

  point_data_attr_3.attributes = {{"key3", "value3"}};
  point_data_attr_3.point_data = s_data_3;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
  point_data_attr.push_back(point_data_attr_3);
  data.point_data_attr_ = std::move(point_data_attr);
  return data;
}

TEST(OtlpMetricSerializationTest, Counter)
{
  metrics_sdk::MetricData data = CreateSumAggregationData();
  opentelemetry::proto::metrics::v1::Sum sum;
  otlp_exporter::OtlpMetricUtils::ConvertSumMetric(data, &sum);
  EXPECT_EQ(sum.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);
  EXPECT_EQ(sum.is_monotonic(), true);
  for (int i = 0; i < 2; i++)
  {
    const auto &proto_number_point = sum.data_points(i);
    EXPECT_EQ(proto_number_point.as_double(), i == 0 ? 10.2 : 20.2);
  }

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, UpDownCounter)
{
  metrics_sdk::MetricData data = CreateUpDownCounterAggregationData();
  opentelemetry::proto::metrics::v1::Sum sum;
  otlp_exporter::OtlpMetricUtils::ConvertSumMetric(data, &sum);
  EXPECT_EQ(sum.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);
  EXPECT_EQ(sum.is_monotonic(), false);
  EXPECT_EQ(sum.data_points(0).as_double(), 1.35);
  EXPECT_EQ(sum.data_points(1).as_double(), 1.37);

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, Histogram)
{
  metrics_sdk::MetricData data = CreateHistogramAggregationData();
  opentelemetry::proto::metrics::v1::Histogram histogram;
  otlp_exporter::OtlpMetricUtils::ConvertHistogramMetric(data, &histogram);
  EXPECT_EQ(histogram.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);
  for (int i = 0; i < 2; i++)
  {
    const auto &proto_number_point = histogram.data_points(i);
    EXPECT_EQ(proto_number_point.sum(), i == 0 ? 100.2 : 200.2);
  }

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, ExponentialHistogramAggregationData)
{
  const auto start_test_time = std::chrono::system_clock::now();
  const auto data            = CreateExponentialHistogramAggregationData(start_test_time);
  opentelemetry::proto::metrics::v1::ExponentialHistogram exponentialHistogram;
  otlp_exporter::OtlpMetricUtils::ConvertExponentialHistogramMetric(data, &exponentialHistogram);
  EXPECT_EQ(exponentialHistogram.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);

  EXPECT_EQ(exponentialHistogram.data_points_size(), 2);
  // Point 1
  {
    const auto &data_point1 = exponentialHistogram.data_points(0);
    EXPECT_EQ(data_point1.count(), 3);
    EXPECT_EQ(data_point1.sum(), 6.5);
    EXPECT_EQ(data_point1.min(), 0.0);
    EXPECT_EQ(data_point1.max(), 3.5);
    EXPECT_EQ(data_point1.zero_count(), 1);
    EXPECT_EQ(data_point1.scale(), 3);
    EXPECT_EQ(data_point1.positive().offset(), 1);
    EXPECT_EQ(data_point1.positive().bucket_counts_size(), 1);
    EXPECT_EQ(data_point1.positive().bucket_counts(0), 1);
    EXPECT_EQ(data_point1.negative().offset(), -2);
    EXPECT_EQ(data_point1.negative().bucket_counts_size(), 1);
    EXPECT_EQ(data_point1.negative().bucket_counts(0), 1);

    EXPECT_EQ(data_point1.attributes_size(), 1);
    EXPECT_EQ(data_point1.attributes(0).key(), "k1");
    EXPECT_EQ(data_point1.attributes(0).value().string_value(), "v1");
    EXPECT_EQ(data_point1.start_time_unix_nano(), data.start_ts.time_since_epoch().count());
    EXPECT_EQ(data_point1.time_unix_nano(), data.end_ts.time_since_epoch().count());
  }

  // Point 2
  {
    const auto &data_point2 = exponentialHistogram.data_points(1);
    EXPECT_EQ(data_point2.count(), 4);
    EXPECT_EQ(data_point2.sum(), 6.2);
    EXPECT_EQ(data_point2.min(), 0.0);
    EXPECT_EQ(data_point2.max(), 0.0);
    EXPECT_EQ(data_point2.zero_count(), 2);
    EXPECT_EQ(data_point2.scale(), 3);
    EXPECT_EQ(data_point2.positive().offset(), 3);
    EXPECT_EQ(data_point2.positive().bucket_counts_size(), 1);
    EXPECT_EQ(data_point2.positive().bucket_counts(0), 1);
    EXPECT_EQ(data_point2.negative().offset(), -4);
    EXPECT_EQ(data_point2.negative().bucket_counts_size(), 3);
    EXPECT_EQ(data_point2.negative().bucket_counts(0), 2);
    EXPECT_EQ(data_point2.negative().bucket_counts(1), 0);
    EXPECT_EQ(data_point2.negative().bucket_counts(2), 1);
    EXPECT_EQ(data_point2.attributes(0).key(), "k2");
    EXPECT_EQ(data_point2.attributes(0).value().string_value(), "v2");
    EXPECT_EQ(data_point2.start_time_unix_nano(), data.start_ts.time_since_epoch().count());
    EXPECT_EQ(data_point2.time_unix_nano(), data.end_ts.time_since_epoch().count());
  }

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, ObservableGauge)
{
  metrics_sdk::MetricData data = CreateObservableGaugeAggregationData();
  opentelemetry::proto::metrics::v1::Gauge gauge;
  otlp_exporter::OtlpMetricUtils::ConvertGaugeMetric(data, &gauge);
  for (int i = 0; i < 2; i++)
  {
    const auto &proto_number_point = gauge.data_points(i);
    EXPECT_EQ(proto_number_point.as_double(), i == 0 ? 30.2 : 50.2);
  }

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, ObservableCounter)
{
  metrics_sdk::MetricData data = CreateObservableCounterAggregationData();
  opentelemetry::proto::metrics::v1::Sum sum;
  otlp_exporter::OtlpMetricUtils::ConvertSumMetric(data, &sum);
  EXPECT_EQ(sum.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);
  EXPECT_EQ(sum.is_monotonic(), true);
  EXPECT_EQ(sum.data_points_size(), 2);
  EXPECT_EQ(sum.data_points(0).as_double(), 1.23);
  EXPECT_EQ(sum.data_points(1).as_double(), 4.56);

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, ObservableUpDownCounter)
{
  metrics_sdk::MetricData data = CreateObservableUpDownCounterAggregationData();
  opentelemetry::proto::metrics::v1::Sum sum;
  otlp_exporter::OtlpMetricUtils::ConvertSumMetric(data, &sum);
  EXPECT_EQ(sum.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);
  EXPECT_EQ(sum.is_monotonic(), false);
  EXPECT_EQ(sum.data_points_size(), 3);
  EXPECT_EQ(sum.data_points(0).as_double(), 1.23);
  EXPECT_EQ(sum.data_points(1).as_double(), 4.56);
  EXPECT_EQ(sum.data_points(2).as_double(), 2.34);

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, PopulateExportMetricsServiceRequest)
{
  const auto resource =
      resource::Resource::Create({{"service.name", "test_service_name"}}, "resource_schema_url");
  const auto scope = opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create(
      "scope_name", "scope_version", "scope_schema_url", {{"scope_key", "scope_value"}});

  metrics_sdk::ScopeMetrics scope_metrics{scope.get(), CreateSumAggregationData()};
  metrics_sdk::ResourceMetrics resource_metrics{&resource, scope_metrics};

  proto::collector::metrics::v1::ExportMetricsServiceRequest request_proto;
  otlp_exporter::OtlpMetricUtils::PopulateRequest(resource_metrics, &request_proto);

  ASSERT_EQ(1, request_proto.resource_metrics_size());
  const auto &resource_metrics_proto = request_proto.resource_metrics(0);
  EXPECT_EQ("resource_schema_url", resource_metrics_proto.schema_url());

  ASSERT_EQ(1, resource_metrics_proto.scope_metrics_size());
  const auto &scope_metrics_proto = resource_metrics_proto.scope_metrics(0);
  EXPECT_EQ("scope_schema_url", scope_metrics_proto.schema_url());

  ASSERT_EQ(1, scope_metrics_proto.metrics_size());
  const auto &metric_proto = scope_metrics_proto.metrics(0);
  EXPECT_EQ("Counter", metric_proto.name());

  const auto &scope_proto = scope_metrics_proto.scope();
  EXPECT_EQ("scope_name", scope_proto.name());
  EXPECT_EQ("scope_version", scope_proto.version());

  ASSERT_EQ(1, scope_proto.attributes_size());
  const auto &scope_attributes_proto = scope_proto.attributes(0);
  EXPECT_EQ("scope_key", scope_attributes_proto.key());
  EXPECT_EQ("scope_value", scope_attributes_proto.value().string_value());
}

TEST(OtlpMetricSerializationTest, AttributeTypes)
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.end_ts   = data.start_ts;
  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.instrument_descriptor   = {"TypesCounter", "desc", "unit",
                                  metrics_sdk::InstrumentType::kCounter,
                                  metrics_sdk::InstrumentValueType::kDouble};

  metrics_sdk::SumPointData point;
  point.value_ = 1.0;

  const std::array<uint8_t, 4> byte_values                             = {0xDE, 0xAD, 0xBE, 0xEF};
  const std::array<bool, 3> bool_values                                = {true, false, true};
  const std::array<int32_t, 3> int32_values                            = {-1, 0, 1};
  const std::array<int64_t, 2> int64_values                            = {-100, 200};
  const std::array<uint32_t, 2> uint32_values                          = {10u, 20u};
  const std::array<uint64_t, 2> uint64_values                          = {300u, 400u};
  const std::array<double, 2> double_values                            = {1.1, 2.2};
  const std::array<opentelemetry::nostd::string_view, 2> string_values = {"foo", "bar"};

  namespace nostd = opentelemetry::nostd;

  metrics_sdk::PointDataAttributes point_data_attributes;
  point_data_attributes.point_data = point;
  point_data_attributes.attributes = {
      {"bool_attr", bool{true}},
      {"int32_attr", int32_t{-7}},
      {"int64_attr", int64_t{-99}},
      {"uint32_attr", uint32_t{42u}},
      {"uint64_attr", uint64_t{1000u}},
      {"double_attr", 3.14},
      {"string_attr", "hello"},
      {"bytes_attr", nostd::span<const uint8_t>{byte_values.data(), byte_values.size()}},
      {"bool_array_attr", nostd::span<const bool>{bool_values.data(), bool_values.size()}},
      {"int32_array_attr", nostd::span<const int32_t>{int32_values.data(), int32_values.size()}},
      {"int64_array_attr", nostd::span<const int64_t>{int64_values.data(), int64_values.size()}},
      {"uint32_array_attr",
       nostd::span<const uint32_t>{uint32_values.data(), uint32_values.size()}},
      {"uint64_array_attr",
       nostd::span<const uint64_t>{uint64_values.data(), uint64_values.size()}},
      {"double_array_attr", nostd::span<const double>{double_values.data(), double_values.size()}},
      {"string_array_attr",
       nostd::span<const nostd::string_view>{string_values.data(), string_values.size()}},
  };
  data.point_data_attr_ = {point_data_attributes};

  const auto resource = resource::Resource::Create({{"service.name", "svc"}});
  const auto scope =
      opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create("lib", "1");

  metrics_sdk::ScopeMetrics scope_metrics{scope.get(), std::move(data)};
  metrics_sdk::ResourceMetrics resource_metrics{&resource, scope_metrics};

  proto::collector::metrics::v1::ExportMetricsServiceRequest request;
  otlp_exporter::OtlpMetricUtils::PopulateRequest(resource_metrics, &request);

  ASSERT_EQ(1, request.resource_metrics_size());
  ASSERT_EQ(1, request.resource_metrics(0).scope_metrics_size());
  ASSERT_EQ(1, request.resource_metrics(0).scope_metrics(0).metrics_size());
  const auto &data_point =
      request.resource_metrics(0).scope_metrics(0).metrics(0).sum().data_points(0);

  std::map<std::string, proto::common::v1::AnyValue> attributes;
  for (const auto &kv : data_point.attributes())
  {
    attributes[kv.key()] = kv.value();
  }

  ASSERT_EQ(attributes.size(), 15u);

  EXPECT_TRUE(attributes.at("bool_attr").bool_value());
  EXPECT_EQ(attributes.at("int32_attr").int_value(), -7);
  EXPECT_EQ(attributes.at("int64_attr").int_value(), -99);
  EXPECT_EQ(attributes.at("uint32_attr").int_value(), 42);
  EXPECT_EQ(attributes.at("uint64_attr").int_value(), 1000);
  EXPECT_DOUBLE_EQ(attributes.at("double_attr").double_value(), 3.14);
  EXPECT_EQ(attributes.at("string_attr").string_value(), "hello");

  {
    const std::string &bytes = attributes.at("bytes_attr").bytes_value();
    ASSERT_EQ(bytes.size(), 4u);
    EXPECT_EQ(static_cast<uint8_t>(bytes[0]), byte_values[0]);
    EXPECT_EQ(static_cast<uint8_t>(bytes[1]), byte_values[1]);
    EXPECT_EQ(static_cast<uint8_t>(bytes[2]), byte_values[2]);
    EXPECT_EQ(static_cast<uint8_t>(bytes[3]), byte_values[3]);
  }
  {
    const auto &array_value = attributes.at("bool_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 3);
    EXPECT_EQ(array_value.values(0).bool_value(), bool_values[0]);
    EXPECT_EQ(array_value.values(1).bool_value(), bool_values[1]);
    EXPECT_EQ(array_value.values(2).bool_value(), bool_values[2]);
  }
  {
    const auto &array_value = attributes.at("int32_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 3);
    EXPECT_EQ(array_value.values(0).int_value(), int32_values[0]);
    EXPECT_EQ(array_value.values(1).int_value(), int32_values[1]);
    EXPECT_EQ(array_value.values(2).int_value(), int32_values[2]);
  }
  {
    const auto &array_value = attributes.at("int64_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 2);
    EXPECT_EQ(array_value.values(0).int_value(), int64_values[0]);
    EXPECT_EQ(array_value.values(1).int_value(), int64_values[1]);
  }
  {
    const auto &array_value = attributes.at("uint32_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 2);
    EXPECT_EQ(array_value.values(0).int_value(), uint32_values[0]);
    EXPECT_EQ(array_value.values(1).int_value(), uint32_values[1]);
  }
  {
    const auto &array_value = attributes.at("uint64_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 2);
    EXPECT_EQ(array_value.values(0).int_value(), static_cast<int64_t>(uint64_values[0]));
    EXPECT_EQ(array_value.values(1).int_value(), static_cast<int64_t>(uint64_values[1]));
  }
  {
    const auto &array_value = attributes.at("double_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 2);
    EXPECT_DOUBLE_EQ(array_value.values(0).double_value(), double_values[0]);
    EXPECT_DOUBLE_EQ(array_value.values(1).double_value(), double_values[1]);
  }
  {
    const auto &array_value = attributes.at("string_array_attr").array_value();
    ASSERT_EQ(array_value.values_size(), 2);
    EXPECT_EQ(array_value.values(0).string_value(), string_values[0]);
    EXPECT_EQ(array_value.values(1).string_value(), string_values[1]);
  }
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
