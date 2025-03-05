// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class OtlpGrpcPushMetricExporterBuilder
{
public:
  OtlpGrpcPushMetricExporterBuilder()          = default;
  virtual ~OtlpGrpcPushMetricExporterBuilder() = default;

  virtual std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::OtlpGrpcPushMetricExporterConfiguration *model)
      const = 0;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
