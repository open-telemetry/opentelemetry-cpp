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

// YAML-SCHEMA: schema/logger_provider.json
// YAML-NODE: ExperimentalEventToSpanEventBridgeLogRecordProcessor
class EventToSpanEventBridgeLogRecordProcessorConfiguration : public LogRecordProcessorConfiguration
{
public:
  void Accept(LogRecordProcessorConfigurationVisitor *visitor) const override
  {
    visitor->VisitEventToSpanEventBridge(this);
  }
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
