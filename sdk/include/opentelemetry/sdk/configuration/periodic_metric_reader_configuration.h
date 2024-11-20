// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// REF: schema/meter_provider.json
// FIXME: https://github.com/open-telemetry/opentelemetry-configuration/issues/138
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
  std::unique_ptr<PushMetricExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
