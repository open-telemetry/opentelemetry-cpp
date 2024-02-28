// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class OtlpMetricExporterConfiguration;
class ConsoleMetricExporterConfiguration;
class PrometheusMetricExporterConfiguration;
class ExtensionMetricExporterConfiguration;

class MetricExporterConfigurationVisitor
{
public:
  MetricExporterConfigurationVisitor()          = default;
  virtual ~MetricExporterConfigurationVisitor() = default;

  virtual void VisitOtlp(const OtlpMetricExporterConfiguration *model)             = 0;
  virtual void VisitConsole(const ConsoleMetricExporterConfiguration *model)       = 0;
  virtual void VisitPrometheus(const PrometheusMetricExporterConfiguration *model) = 0;
  virtual void VisitExtension(const ExtensionMetricExporterConfiguration *model)   = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
