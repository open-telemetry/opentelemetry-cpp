// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/otlp_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/init/otlp_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OPENTELEMETRY_EXPORT OtlpHttpPushMetricBuilder
    : public opentelemetry::sdk::init::OtlpPushMetricExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  OtlpHttpPushMetricBuilder()           = default;
  ~OtlpHttpPushMetricBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::OtlpPushMetricExporterConfiguration *model)
      const override;
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
