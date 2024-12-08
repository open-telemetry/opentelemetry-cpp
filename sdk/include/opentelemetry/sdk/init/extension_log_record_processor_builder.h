// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/extension_log_record_processor_configuration.h"
#include "opentelemetry/sdk/logs/processor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{

class ExtensionLogRecordProcessorBuilder
{
public:
  ExtensionLogRecordProcessorBuilder()          = default;
  virtual ~ExtensionLogRecordProcessorBuilder() = default;

  virtual std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const opentelemetry::sdk::configuration::ExtensionLogRecordProcessorConfiguration *model)
      const = 0;
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE