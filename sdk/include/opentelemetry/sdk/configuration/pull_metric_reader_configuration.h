// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration_visitor.h"
#include "opentelemetry/sdk/configuration/pull_metric_exporter_configuration.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class PullMetricReaderConfiguration : public MetricReaderConfiguration
{
public:
  PullMetricReaderConfiguration()           = default;
  ~PullMetricReaderConfiguration() override = default;

  void Accept(MetricReaderConfigurationVisitor *visitor) const override
  {
    visitor->VisitPull(this);
  }

  std::unique_ptr<PullMetricExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
