// Copyright 2023, OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

struct PrometheusPushExporterOptions;

/**
 * Factory class for PrometheusExporter.
 */
class PrometheusPushExporterFactory
{
public:
  /**
   * Create a PrometheusExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Create(
      const PrometheusPushExporterOptions &options);
};

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
