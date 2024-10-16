// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_processor_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class BatchLogRecordProcessorConfiguration : public LogRecordProcessorConfiguration
{
public:
  BatchLogRecordProcessorConfiguration()           = default;
  ~BatchLogRecordProcessorConfiguration() override = default;

  void Accept(LogRecordProcessorConfigurationVisitor *visitor) const override
  {
    visitor->VisitBatch(this);
  }

  size_t schedule_delay;
  size_t export_timeout;
  size_t max_queue_size;
  size_t max_export_batch_size;
  std::unique_ptr<LogRecordExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
