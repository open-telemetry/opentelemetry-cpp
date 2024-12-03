// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class ZipkinSpanExporterConfiguration : public SpanExporterConfiguration
{
public:
  ZipkinSpanExporterConfiguration()           = default;
  ~ZipkinSpanExporterConfiguration() override = default;

  void Accept(SpanExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitZipkin(this);
  }

  std::string endpoint;
  size_t timeout;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE