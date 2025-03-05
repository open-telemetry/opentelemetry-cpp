// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/log_record_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/common.json
// YAML-NODE: Otlp
class OtlpGrpcLogRecordExporterConfiguration : public LogRecordExporterConfiguration
{
public:
  OtlpGrpcLogRecordExporterConfiguration()           = default;
  ~OtlpGrpcLogRecordExporterConfiguration() override = default;

  void Accept(LogRecordExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitOtlpGrpc(this);
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
