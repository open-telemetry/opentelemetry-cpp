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

inline bool GetPrometheusWithoutOtelScope()
{
  constexpr char kPrometheusWithoutOtelScope[] = "OTEL_CPP_PROMETHEUS_EXPORTER_WITHOUT_OTEL_SCOPE";

  bool setting;
  auto exists =
      opentelemetry::sdk::common::GetBoolEnvironmentVariable(kPrometheusWithoutOtelScope, setting);

  return exists ? setting : true;
}

inline bool GetPrometheusPopulateTargetInfo()
{
  constexpr char kPrometheusPopulateTargetInfo[] =
      "OTEL_CPP_PROMETHEUS_EXPORTER_POPULATE_TARGET_INFO";

  bool setting;
  auto exists = opentelemetry::sdk::common::GetBoolEnvironmentVariable(
      kPrometheusPopulateTargetInfo, setting);

  return exists ? setting : true;
}

PrometheusExporterOptions::PrometheusExporterOptions()
    : url(GetPrometheusDefaultHttpEndpoint()),
      populate_target_info(GetPrometheusPopulateTargetInfo()),
      without_otel_scope(GetPrometheusWithoutOtelScope())
{}

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
