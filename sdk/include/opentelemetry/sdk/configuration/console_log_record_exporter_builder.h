// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/configuration/console_log_record_exporter_configuration.h"
#include "opentelemetry/sdk/logs/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ConsoleLogRecordExporterBuilder
{
public:
  ConsoleLogRecordExporterBuilder()                                              = default;
  ConsoleLogRecordExporterBuilder(ConsoleLogRecordExporterBuilder &&)            = default;
  ConsoleLogRecordExporterBuilder(const ConsoleLogRecordExporterBuilder &)       = default;
  ConsoleLogRecordExporterBuilder &operator=(ConsoleLogRecordExporterBuilder &&) = default;
  ConsoleLogRecordExporterBuilder &operator=(const ConsoleLogRecordExporterBuilder &other) =
      default;
  virtual ~ConsoleLogRecordExporterBuilder() = default;

  virtual std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const opentelemetry::sdk::configuration::ConsoleLogRecordExporterConfiguration *model)
      const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
