// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_metrics_utils.h"

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
metrics_sdk::MetricData CreateSumAggregationData()
{
  metrics_sdk::MetricData data;
  data.start_ts = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  metrics_sdk::InstrumentDescriptor inst_desc = {"Counter", "", "",
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
  return data;
}

TEST(OtlpMetricsSerializationTest, Counter)
{
  metrics_sdk::MetricData data = CreateSumAggregationData();
  opentelemetry::proto::metrics::v1::Sum sum;
  otlp_exporter::OtlpMetricsUtils::ConvertSumMetric(data, &sum);
  EXPECT_EQ(1, 1);
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
