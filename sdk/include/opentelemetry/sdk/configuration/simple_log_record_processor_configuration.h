// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class SimpleLogRecordProcessorConfiguration : public LogRecordProcessorConfiguration
{
public:
  SimpleLogRecordProcessorConfiguration()           = default;
  ~SimpleLogRecordProcessorConfiguration() override = default;

  void Accept(LogRecordProcessorConfigurationVisitor *visitor) const override
  {
    visitor->VisitSimple(this);
  }

  std::unique_ptr<LogRecordExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
