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
  opentelemetry::sdk::metrics::AggregationTemporality aggregation_temporality =
      opentelemetry::sdk::metrics::AggregationTemporality::kDelta;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE