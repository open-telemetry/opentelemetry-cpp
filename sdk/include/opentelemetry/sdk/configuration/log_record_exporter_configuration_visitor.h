// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class OtlpLogRecordExporterConfiguration;
class ConsoleLogRecordExporterConfiguration;
class ExtensionLogRecordExporterConfiguration;

class LogRecordExporterConfigurationVisitor
{
public:
  LogRecordExporterConfigurationVisitor()          = default;
  virtual ~LogRecordExporterConfigurationVisitor() = default;

  virtual void VisitOtlp(const OtlpLogRecordExporterConfiguration *model)           = 0;
  virtual void VisitConsole(const ConsoleLogRecordExporterConfiguration *model)     = 0;
  virtual void VisitExtension(const ExtensionLogRecordExporterConfiguration *model) = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
