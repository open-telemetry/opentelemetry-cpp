// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_metric_utils.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"

#ifndef ENABLE_METRICS_PREVIEW

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace otlp
{
namespace metric_sdk = opentelemetry::sdk::metrics;

proto::metrics::v1::AggregationTemporality OtlpMetricUtils::GetProtoAggregationTemporality(
    const opentelemetry::sdk::metrics::AggregationTemporality &aggregation_temporality) noexcept
{
  if (aggregation_temporality == opentelemetry::sdk::metrics::AggregationTemporality::kCumulative)
    return proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE;
  else if (aggregation_temporality == opentelemetry::sdk::metrics::AggregationTemporality::kDelta)
    return proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_DELTA;
  else
    return proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_UNSPECIFIED;
}

metric_sdk::AggregationType OtlpMetricUtils::GetAggregationType(
    const opentelemetry::sdk::metrics::InstrumentType &instrument_type) noexcept
{

  if (instrument_type == metric_sdk::InstrumentType::kCounter ||
      instrument_type == metric_sdk::InstrumentType::kUpDownCounter ||
      instrument_type == metric_sdk::InstrumentType::kObservableCounter ||
      instrument_type == metric_sdk::InstrumentType::kObservableUpDownCounter)
  {
    return metric_sdk::AggregationType::kSum;
  }
  else if (instrument_type == metric_sdk::InstrumentType::kHistogram)
  {
    return metric_sdk::AggregationType::kHistogram;
  }
  else if (instrument_type == metric_sdk::InstrumentType::kObservableGauge)
  {
    return metric_sdk::AggregationType::kLastValue;
  }
  return metric_sdk::AggregationType::kDrop;
}

void OtlpMetricUtils::ConvertSumMetric(const metric_sdk::MetricData &metric_data,
                                       proto::metrics::v1::Sum *const sum) noexcept
{
  sum->set_aggregation_temporality(
      GetProtoAggregationTemporality(metric_data.aggregation_temporality));
  sum->set_is_monotonic(true);
  auto start_ts = metric_data.start_ts.time_since_epoch().count();
  auto ts       = metric_data.end_ts.time_since_epoch().count();
  for (auto &point_data_with_attributes : metric_data.point_data_attr_)
  {
    proto::metrics::v1::NumberDataPoint *proto_sum_point_data = sum->add_data_points();
    proto_sum_point_data->set_start_time_unix_nano(start_ts);
    proto_sum_point_data->set_time_unix_nano(ts);
    auto sum_data = nostd::get<sdk::metrics::SumPointData>(point_data_with_attributes.point_data);

    if ((nostd::holds_alternative<long>(sum_data.value_)))
    {
      proto_sum_point_data->set_as_int(nostd::get<long>(sum_data.value_));
    }
    else
    {
      proto_sum_point_data->set_as_double(nostd::get<double>(sum_data.value_));
    }
    // set attributes
    for (auto &kv_attr : point_data_with_attributes.attributes)
    {
      OtlpPopulateAttributeUtils::PopulateAttribute(proto_sum_point_data->add_attributes(),
                                                    kv_attr.first, kv_attr.second);
    }
  }
}

void OtlpMetricUtils::ConvertHistogramMetric(
    const metric_sdk::MetricData &metric_data,
    proto::metrics::v1::Histogram *const histogram) noexcept
{
  histogram->set_aggregation_temporality(
      GetProtoAggregationTemporality(metric_data.aggregation_temporality));
  auto start_ts = metric_data.start_ts.time_since_epoch().count();
  auto ts       = metric_data.end_ts.time_since_epoch().count();
  for (auto &point_data_with_attributes : metric_data.point_data_attr_)
  {
    proto::metrics::v1::HistogramDataPoint *proto_histogram_point_data =
        histogram->add_data_points();
    proto_histogram_point_data->set_start_time_unix_nano(start_ts);
    proto_histogram_point_data->set_time_unix_nano(ts);
    auto histogram_data =
        nostd::get<sdk::metrics::HistogramPointData>(point_data_with_attributes.point_data);
    // sum
    if ((nostd::holds_alternative<long>(histogram_data.sum_)))
    {
      proto_histogram_point_data->set_sum(nostd::get<long>(histogram_data.sum_));
    }
    else
    {
      proto_histogram_point_data->set_sum(nostd::get<double>(histogram_data.sum_));
    }
    // count
    proto_histogram_point_data->set_count(histogram_data.count_);
    // buckets
    if ((nostd::holds_alternative<std::list<double>>(histogram_data.boundaries_)))
    {
      auto boundaries = nostd::get<std::list<double>>(histogram_data.boundaries_);
      for (auto bound : boundaries)
      {
        proto_histogram_point_data->add_explicit_bounds(bound);
      }
    }
    else
    {
      auto boundaries = nostd::get<std::list<long>>(histogram_data.boundaries_);
      for (auto bound : boundaries)
      {
        proto_histogram_point_data->add_explicit_bounds(bound);
      }
    }
    // bucket counts
    for (auto bucket_value : histogram_data.counts_)
    {
      proto_histogram_point_data->add_bucket_counts(bucket_value);
    }
    // attributes
    for (auto &kv_attr : point_data_with_attributes.attributes)
    {
      OtlpPopulateAttributeUtils::PopulateAttribute(proto_histogram_point_data->add_attributes(),
                                                    kv_attr.first, kv_attr.second);
    }
  }
}

void OtlpMetricUtils::ConvertGaugeMetric(const opentelemetry::sdk::metrics::MetricData &metric_data,
                                         proto::metrics::v1::Gauge *const gauge) noexcept
{
  auto start_ts = metric_data.start_ts.time_since_epoch().count();
  auto ts       = metric_data.end_ts.time_since_epoch().count();
  for (auto &point_data_with_attributes : metric_data.point_data_attr_)
  {
    proto::metrics::v1::NumberDataPoint *proto_gauge_point_data = gauge->add_data_points();
    proto_gauge_point_data->set_start_time_unix_nano(start_ts);
    proto_gauge_point_data->set_time_unix_nano(ts);
    auto gauge_data =
        nostd::get<sdk::metrics::LastValuePointData>(point_data_with_attributes.point_data);

    if ((nostd::holds_alternative<long>(gauge_data.value_)))
    {
      proto_gauge_point_data->set_as_int(nostd::get<long>(gauge_data.value_));
    }
    else
    {
      proto_gauge_point_data->set_as_double(nostd::get<double>(gauge_data.value_));
    }
    // set attributes
    for (auto &kv_attr : point_data_with_attributes.attributes)
    {
      OtlpPopulateAttributeUtils::PopulateAttribute(proto_gauge_point_data->add_attributes(),
                                                    kv_attr.first, kv_attr.second);
    }
  }
}

void OtlpMetricUtils::PopulateInstrumentInfoMetrics(
    const opentelemetry::sdk::metrics::MetricData &metric_data,
    proto::metrics::v1::Metric *metric) noexcept
{
  metric->set_name(metric_data.instrument_descriptor.name_);
  metric->set_description(metric_data.instrument_descriptor.description_);
  metric->set_unit(metric_data.instrument_descriptor.unit_);
  auto kind = GetAggregationType(metric_data.instrument_descriptor.type_);
  switch (kind)
  {
    case metric_sdk::AggregationType::kSum: {
      ConvertSumMetric(metric_data, metric->mutable_sum());
      break;
    }
    case metric_sdk::AggregationType::kHistogram: {
      ConvertHistogramMetric(metric_data, metric->mutable_histogram());
      break;
    }
    case metric_sdk::AggregationType::kLastValue: {
      ConvertGaugeMetric(metric_data, metric->mutable_gauge());
      break;
    }
    default:
      break;
  }
}

void OtlpMetricUtils::PopulateResourceMetrics(
    const opentelemetry::sdk::metrics::ResourceMetrics &data,
    proto::metrics::v1::ResourceMetrics *resource_metrics) noexcept
{
  OtlpPopulateAttributeUtils::PopulateAttribute(resource_metrics->mutable_resource(),
                                                *(data.resource_));

  for (auto &scope_metrics : data.scope_metric_data_)
  {
    if (scope_metrics.scope_ == nullptr)
    {
      continue;
    }
    auto scope_lib_metrics                         = resource_metrics->add_scope_metrics();
    proto::common::v1::InstrumentationScope *scope = scope_lib_metrics->mutable_scope();
    scope->set_name(scope_metrics.scope_->GetName());
    scope->set_version(scope_metrics.scope_->GetVersion());

    for (auto &metric_data : scope_metrics.metric_data_)
    {
      PopulateInstrumentInfoMetrics(metric_data, scope_lib_metrics->add_metrics());
    }
  }
}

void OtlpMetricUtils::PopulateRequest(
    const opentelemetry::sdk::metrics::ResourceMetrics &data,
    proto::collector::metrics::v1::ExportMetricsServiceRequest *request) noexcept
{
  if (request == nullptr || data.resource_ == nullptr)
  {
    return;
  }

  auto resource_metrics = request->add_resource_metrics();
  PopulateResourceMetrics(data, resource_metrics);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
