// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_metric_utils.h"
#  include "opentelemetry/proto/metrics/v1/metrics.pb.h"

#  include <gtest/gtest.h>

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
  point_data_attr_2.point_data = s_data_1;
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
  s_data_1.boundaries_ = std::list<double>({0.0, 10.0, 20.0, 30.0});
  s_data_2.sum_        = 200.2;
  s_data_2.count_      = 20;
  s_data_2.counts_     = {0, 8, 5, 7};
  s_data_2.boundaries_ = std::list<double>({0.0, 10.0, 20.0, 30.0});

  data.aggregation_temporality = metrics_sdk::AggregationTemporality::kCumulative;
  data.end_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  data.instrument_descriptor = inst_desc;
  metrics_sdk::PointDataAttributes point_data_attr_1, point_data_attr_2;
  point_data_attr_1.attributes = {{"k1", "v1"}};
  point_data_attr_1.point_data = s_data_1;

  point_data_attr_2.attributes = {{"k2", "v2"}};
  point_data_attr_2.point_data = s_data_1;
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
  point_data_attr_2.point_data = s_data_1;
  std::vector<metrics_sdk::PointDataAttributes> point_data_attr;
  point_data_attr.push_back(point_data_attr_1);
  point_data_attr.push_back(point_data_attr_2);
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
  for (size_t i = 0; i < 1; i++)
  {
    auto proto_number_point = sum.data_points(i);
    EXPECT_EQ(proto_number_point.as_double(), i == 0 ? 10.2 : 20.2);
  }

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, Histogram)
{
  metrics_sdk::MetricData data = CreateHistogramAggregationData();
  opentelemetry::proto::metrics::v1::Histogram histogram;
  otlp_exporter::OtlpMetricUtils::ConvertHistogramMetric(data, &histogram);
  EXPECT_EQ(histogram.aggregation_temporality(),
            proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE);
  for (size_t i = 0; i < 1; i++)
  {
    auto proto_number_point = histogram.data_points(i);
    EXPECT_EQ(proto_number_point.sum(), i == 0 ? 100.2 : 200.2);
  }

  EXPECT_EQ(1, 1);
}

TEST(OtlpMetricSerializationTest, ObservableGauge)
{
  metrics_sdk::MetricData data = CreateObservableGaugeAggregationData();
  opentelemetry::proto::metrics::v1::Gauge gauge;
  otlp_exporter::OtlpMetricUtils::ConvertGaugeMetric(data, &gauge);
  for (size_t i = 0; i < 1; i++)
  {
    auto proto_number_point = gauge.data_points(i);
    EXPECT_EQ(proto_number_point.as_double(), i == 0 ? 30.2 : 50.2);
  }

  EXPECT_EQ(1, 1);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
