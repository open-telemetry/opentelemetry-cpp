// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/span_processor_configuration.h"
#include "opentelemetry/sdk/configuration/span_processor_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class SimpleSpanProcessorConfiguration : public SpanProcessorConfiguration
{
public:
  SimpleSpanProcessorConfiguration()           = default;
  ~SimpleSpanProcessorConfiguration() override = default;

  void Accept(SpanProcessorConfigurationVisitor *visitor) const override
  {
    visitor->VisitSimple(this);
  }

  std::unique_ptr<SpanExporterConfiguration> exporter;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
