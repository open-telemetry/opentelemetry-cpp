// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class MetricExporterConfigurationVisitor;

class MetricExporterConfiguration
{
public:
  MetricExporterConfiguration()          = default;
  virtual ~MetricExporterConfiguration() = default;

  virtual void Accept(MetricExporterConfigurationVisitor *visitor) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
