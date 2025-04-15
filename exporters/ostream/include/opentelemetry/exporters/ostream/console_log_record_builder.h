// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/init/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/init/registry.h"
#include "opentelemetry/sdk/logs/exporter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{

class OPENTELEMETRY_EXPORT ConsoleLogRecordBuilder
    : public opentelemetry::sdk::init::ConsoleLogRecordExporterBuilder
{
public:
  static void Register(opentelemetry::sdk::init::Registry *registry);

  ConsoleLogRecordBuilder()           = default;
  ~ConsoleLogRecordBuilder() override = default;

  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *model)
      const override;
};

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
