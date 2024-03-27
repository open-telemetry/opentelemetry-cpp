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

class ConsoleMetricExporterConfiguration : public MetricExporterConfiguration
{
public:
  ConsoleMetricExporterConfiguration()           = default;
  ~ConsoleMetricExporterConfiguration() override = default;

  void Accept(MetricExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitConsole(this);
  }
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
