// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{
class LogRecordExporterConfigurationVisitor;

// YAML-SCHEMA: schema/logger_provider.json
// YAML-NODE: LogRecordExporter
class LogRecordExporterConfiguration
{
public:
  LogRecordExporterConfiguration()          = default;
  virtual ~LogRecordExporterConfiguration() = default;

  virtual void Accept(LogRecordExporterConfigurationVisitor *visitor) const = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
