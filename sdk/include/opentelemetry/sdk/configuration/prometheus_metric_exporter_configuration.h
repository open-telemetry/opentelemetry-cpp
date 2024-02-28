// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class PrometheusMetricExporterConfiguration : public MetricExporterConfiguration
{
public:
  PrometheusMetricExporterConfiguration()           = default;
  ~PrometheusMetricExporterConfiguration() override = default;

  void Accept(MetricExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitPrometheus(this);
  }

  std::string host;
  size_t port;
  bool without_units;
  bool without_type_suffix;
  bool without_scope_info;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
