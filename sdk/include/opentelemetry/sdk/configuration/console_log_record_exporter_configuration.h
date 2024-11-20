// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// REF: schema/common.json, Console
class ConsoleLogRecordExporterConfiguration : public LogRecordExporterConfiguration
{
public:
  ConsoleLogRecordExporterConfiguration()           = default;
  ~ConsoleLogRecordExporterConfiguration() override = default;

  void Accept(LogRecordExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitConsole(this);
  }
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
