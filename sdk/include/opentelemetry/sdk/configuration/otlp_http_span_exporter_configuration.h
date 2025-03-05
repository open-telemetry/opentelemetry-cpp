// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/span_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/common.json
// YAML-NODE: OtlpHttpExporter
class OtlpHttpSpanExporterConfiguration : public SpanExporterConfiguration
{
public:
  OtlpHttpSpanExporterConfiguration()           = default;
  ~OtlpHttpSpanExporterConfiguration() override = default;

  void Accept(SpanExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitOtlpHttp(this);
  }

  std::string protocol;
  std::string endpoint;
  std::string certificate;
  std::string client_key;
  std::string client_certificate;
  std::unique_ptr<HeadersConfiguration> headers;
  std::string headers_list;
  std::string compression;
  size_t timeout{0};
  bool insecure{false};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
