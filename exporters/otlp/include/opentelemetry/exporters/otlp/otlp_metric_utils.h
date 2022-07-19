// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
#include "opentelemetry/proto/metrics/v1/metrics.pb.h"
#include "opentelemetry/proto/resource/v1/resource.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/sdk/metrics/export/metric_producer.h"

#ifndef ENABLE_METRICS_PREVIEW

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
/**
 * The OtlpMetricUtils contains utility functions for OTLP metrics
 */
class OtlpMetricUtils
{
public:
  static opentelemetry::sdk::metrics::AggregationType GetAggregationType(
      const opentelemetry::sdk::metrics::InstrumentType &instrument_type) noexcept;

  static proto::metrics::v1::AggregationTemporality GetProtoAggregationTemporality(
      const opentelemetry::sdk::metrics::AggregationTemporality &aggregation_temporality) noexcept;

  static void ConvertSumMetric(const opentelemetry::sdk::metrics::MetricData &metric_data,
                               proto::metrics::v1::Sum *const sum) noexcept;

  static void ConvertHistogramMetric(const opentelemetry::sdk::metrics::MetricData &metric_data,
                                     proto::metrics::v1::Histogram *const histogram) noexcept;

  static void ConvertGaugeMetric(const opentelemetry::sdk::metrics::MetricData &metric_data,
                                 proto::metrics::v1::Gauge *const gauge) noexcept;

  static void PopulateInstrumentationInfoMetric(
      const opentelemetry::sdk::metrics::MetricData &metric_data,
      proto::metrics::v1::Metric *metric) noexcept;

  static void PopulateResourceMetrics(
      const opentelemetry::sdk::metrics::ResourceMetrics &data,
      proto::metrics::v1::ResourceMetrics *proto_resource_metrics) noexcept;

  static void PopulateRequest(
      const opentelemetry::sdk::metrics::ResourceMetrics &data,
      proto::collector::metrics::v1::ExportMetricsServiceRequest *request) noexcept;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
