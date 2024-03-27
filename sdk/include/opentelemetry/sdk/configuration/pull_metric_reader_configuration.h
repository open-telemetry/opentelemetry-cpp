// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/metric_reader_configuration.h"
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

  std::unique_ptr<MetricExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
