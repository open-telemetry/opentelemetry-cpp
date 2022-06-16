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
 * The OtlpMetricsUtils contains utility functions for OTLP metrics
 */
class OtlpMetricsUtils
{
public:
  static opentelemetry::sdk::metrics::AggregationType GetAggregationType(
      const opentelemetry::sdk::metrics::InstrumentType &instrument_type) noexcept;

  static proto::metrics::v1::AggregationTemporality ConvertAggregationTemporality(
      const opentelemetry::sdk::metrics::AggregationTemporality &aggregation_temporality) noexcept;

  static void ConvertSumMetric(const metric_sdk::MetricData &metric_data,
                               const proto::metrics::v1::Sum *sum) noexcept;

  static void ConvertHistogramMetric(const metric_sdk::MetricData &metric_data,
                                     const proto::metrics::v1::Histogram *histogram) noexcept;

  static void PopulateRequest(
      const opentelemetry::sdk::metrics::ResourceMetrics &data,
      proto::collector::metrics::v1::ExportMetricsServiceRequest *request) noexcept;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif
