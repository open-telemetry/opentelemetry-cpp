// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/init/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/logs/processor.h"

class CustomLogRecordProcessorBuilder
    : public opentelemetry::sdk::init::ExtensionLogRecordProcessorBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model)
      const override;
};
