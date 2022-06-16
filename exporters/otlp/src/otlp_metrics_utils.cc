// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_metrics_utils.h"
#include "opentelemetry/exporters/otlp/otlp_populate_attribute_utils.h"

#ifndef ENABLE_METRICS_PREVIEW

OPENTELEMETRY_BEGIN_NAMESPACE

namespace exporter
{
namespace otlp
{
namespace metric_sdk = opentelemetry::sdk::metrics;

proto::metrics::v1::AggregationTemporality OtlpMetricsUtils::ConvertAggregationTemporality(
    const opentelemetry::sdk::metrics::AggregationTemporality &aggregation_temporality) noexcept
{
  if (aggregation_temporality == opentelemetry::sdk::metrics::AggregationTemporality::kCumulative)
    return proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_CUMULATIVE;
  else if (aggregation_temporality == opentelemetry::sdk::metrics::AggregationTemporality::kDelta)
    return proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_DELTA;
  else
    return proto::metrics::v1::AggregationTemporality::AGGREGATION_TEMPORALITY_UNSPECIFIED;
}

metric_sdk::AggregationType OtlpMetricsUtils::GetAggregationType(
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

void OtlpMetricsUtils::ConvertSumMetric(const metric_sdk::MetricData &metric_data,
                                        const proto::metrics::v1::Sum *sum) noexcept
{
  auto ts = metric_data.start_ts(;) sum->for (auto &point_data : metric_data.point_data_attr_)
  {
    proto::metrics::v1::NumberDataPoint proto_point_data;
    proto_point_data.set_start_time_unix_nano()
  }
  // sum->set_aggregation_temporality(ConvertAggregationTemporality(point_data_list.));
}

void OtlpMetricsUtils::ConvertHistogramMetric(
    const metric_sdk::MetricData &metric_data,
    const proto::metrics::v1::Histogram *histogram) noexcept
{}

void OtlpMetricsUtils::PopulateRequest(
    const opentelemetry::sdk::metrics::ResourceMetrics &data,
    proto::collector::metrics::v1::ExportMetricsServiceRequest *request) noexcept
{
  if (request == nullptr || data.resource_ == nullptr)
  {
    return;
  }

  // populate resource
  auto resource_metrics = request->add_resource_metrics();
  proto::resource::v1::Resource proto;
  OtlpPopulateAttributeUtils::PopulateAttribute(&proto, *(data.resource_));
  *resource_metrics->mutable_resource() = proto;

  for (auto &instrumentation_metrics : data.instrumentation_info_metric_data_)
  {
    if (instrumentation_metrics.instrumentation_library_ == nullptr)
    {
      continue;
    }
    auto instrumentation_lib_metrics = resource_metrics->add_instrumentation_library_metrics();
    proto::common::v1::InstrumentationLibrary instrumentation_library;
    instrumentation_library.set_name(instrumentation_metrics.instrumentation_library_->GetName());
    instrumentation_library.set_version(
        instrumentation_metrics.instrumentation_library_->GetVersion());
    *instrumentation_lib_metrics->mutable_instrumentation_library() = instrumentation_library;

    for (auto &metric_data : instrumentation_metrics.metric_data_)
    {
      proto::metrics::v1::Metric metric;
      metric.set_name(metric_data.instrument_descriptor.name_);
      metric.set_description(metric_data.instrument_descriptor.description_);
      metric.set_unit(metric_data.instrument_descriptor.unit_);
      auto kind = GetAggregationType(metric_data.instrument_descriptor.type_);
      if (kind == metric_sdk::AggregationType::kSum)
      {
        proto::metrics::v1::Sum sum;
        sum.set_aggregation_temporality(
            ConvertAggregationTemporality(metric_data.aggregation_temporality));
        sum.set_is_monotonic(true);
        ConvertSumMetric(metric_data.point_data_attr_, &sum);
      }
      else if (kind == metric_sdk::AggregationType::kHistogram)
      {
        proto::metrics::v1::Histogram histogram;
        ConvertHistogramMetric(metric_data.point_data_attr_, &histogram);
      }
    }
  }
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
