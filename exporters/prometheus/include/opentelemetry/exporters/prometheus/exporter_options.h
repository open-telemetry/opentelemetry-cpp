// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

/**
 * Struct to hold Prometheus exporter options.
 */
struct PrometheusExporterOptions
{
  PrometheusExporterOptions();

  // The endpoint the Prometheus backend can collect metrics from
  std::string url;

  // Populating target_info
  bool populate_target_info = true;

  // Populating otel_scope_name/otel_scope_labels attributes
  bool without_otel_scope = false;
};

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
