// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/tracer_provider.json
// YAML-NODE: BatchSpanProcessor
class BatchSpanProcessorConfiguration : public SpanProcessorConfiguration
{
public:
  BatchSpanProcessorConfiguration()           = default;
  ~BatchSpanProcessorConfiguration() override = default;

  void Accept(SpanProcessorConfigurationVisitor *visitor) const override
  {
    visitor->VisitBatch(this);
  }

  size_t schedule_delay{0};
  size_t export_timeout{0};
  size_t max_queue_size{0};
  size_t max_export_batch_size{0};
  std::unique_ptr<SpanExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
