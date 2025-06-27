// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/tracer_provider.json
// YAML-NODE: Zipkin
class ZipkinSpanExporterConfiguration : public SpanExporterConfiguration
{
public:
  void Accept(SpanExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitZipkin(this);
  }

  std::string endpoint;
  std::size_t timeout{0};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
