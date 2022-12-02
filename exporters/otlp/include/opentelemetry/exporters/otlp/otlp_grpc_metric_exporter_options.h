// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/sdk/metrics/instruments.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP metrics exporter options.
 */
struct OtlpGrpcMetricExporterOptions : public OtlpGrpcExporterOptions
{

  // Preferred Aggregation Temporality
  sdk::metrics::AggregationTemporality aggregation_temporality =
      sdk::metrics::AggregationTemporality::kCumulative;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
