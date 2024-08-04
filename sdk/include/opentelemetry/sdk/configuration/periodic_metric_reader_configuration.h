// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class PeriodicMetricReaderConfiguration : public MetricReaderConfiguration
{
public:
  PeriodicMetricReaderConfiguration()           = default;
  ~PeriodicMetricReaderConfiguration() override = default;

  void Accept(MetricReaderConfigurationVisitor *visitor) const override
  {
    visitor->VisitPeriodic(this);
  }

  size_t interval;
  size_t timeout;
  std::unique_ptr<MetricExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
