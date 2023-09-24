// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <prometheus/metric_family.h>
#include <string>
#include <vector>
#include "opentelemetry/metrics/provider.h"
#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
/**
 * Convert OpenTelemetry metrics data collection to Prometheus metrics data collection
 *
 * @param data a collection of metrics in OpenTelemetry
 * @return a collection of translated metrics that is acceptable by Prometheus
 */
std::vector<::prometheus::MetricFamily> TranslateToPrometheus(
    const sdk::metrics::ResourceMetrics &data);
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
