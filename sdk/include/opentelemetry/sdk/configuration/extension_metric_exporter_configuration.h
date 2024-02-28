// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/document_node.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ExtensionMetricExporterConfiguration : public MetricExporterConfiguration
{
public:
  ExtensionMetricExporterConfiguration()           = default;
  ~ExtensionMetricExporterConfiguration() override = default;

  void Accept(MetricExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitExtension(this);
  }

  std::string name;
  std::unique_ptr<DocumentNode> node;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
