// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/init/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

class CustomPushMetricExporterBuilder
    : public opentelemetry::sdk::init::ExtensionPushMetricExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionPushMetricExporterConfiguration *model)
      const override;
};
