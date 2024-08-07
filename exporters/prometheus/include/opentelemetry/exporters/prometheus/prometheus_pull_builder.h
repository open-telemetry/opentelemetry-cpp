// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_configuration.h"
#include "opentelemetry/sdk/init/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

class OPENTELEMETRY_EXPORT PrometheusPullBuilder
    : public opentelemetry::sdk::init::PrometheusPullMetricExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  PrometheusPullBuilder()           = default;
  ~PrometheusPullBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const opentelemetry::sdk::configuration::PrometheusPullMetricExporterConfiguration *model)
      const override;
};

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
