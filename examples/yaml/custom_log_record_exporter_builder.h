// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/extension_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/init/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/logs/exporter.h"

class CustomLogRecordExporterBuilder
    : public opentelemetry::sdk::init::ExtensionLogRecordExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ExtensionLogRecordExporterConfiguration *model)
      const override;
};
