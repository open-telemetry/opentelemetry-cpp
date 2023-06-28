// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>

#include "opentelemetry/exporters/prometheus/exporter_options.h"
#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

inline const std::string GetPrometheusDefaultHttpEndpoint()
{
  constexpr char kPrometheusEndpointEnv[]     = "PROMETHEUS_EXPORTER_ENDPOINT";
  constexpr char kPrometheusEndpointDefault[] = "localhost:9464";

  std::string endpoint;

  auto exists =
      opentelemetry::sdk::common::GetStringEnvironmentVariable(kPrometheusEndpointEnv, endpoint);
  return exists ? endpoint : kPrometheusEndpointDefault;
}

PrometheusExporterOptions::PrometheusExporterOptions() : url(GetPrometheusDefaultHttpEndpoint()) {}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
