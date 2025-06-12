// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/logger_provider.json
// YAML-NODE: BatchLogRecordProcessor
class BatchLogRecordProcessorConfiguration : public LogRecordProcessorConfiguration
{
public:
  BatchLogRecordProcessorConfiguration()           = default;
  ~BatchLogRecordProcessorConfiguration() override = default;

  void Accept(LogRecordProcessorConfigurationVisitor *visitor) const override
  {
    visitor->VisitBatch(this);
  }

  size_t schedule_delay{0};
  size_t export_timeout{0};
  size_t max_queue_size{0};
  size_t max_export_batch_size{0};
  std::unique_ptr<LogRecordExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
