// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "opentelemetry/sdk/configuration/grpc_tls_configuration.h"
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
  static constexpr std::size_t kDefaultTimeoutMs = 10000;
  static constexpr std::size_t kMaxRequestSize   = 64 * 1024 * 1024;  // 64 MiB
  static constexpr std::size_t kMaxResponseSize  = 4 * 1024 * 1024;   // 4 MiB

  void Accept(LogRecordExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitOtlpGrpc(this);
  }

  std::string endpoint;
  std::unique_ptr<GrpcTlsConfiguration> tls;
  std::unique_ptr<HeadersConfiguration> headers;
  std::string headers_list;
  std::string compression;
  std::size_t max_request_size{kMaxRequestSize};
  std::size_t max_response_size{kMaxResponseSize};
  std::size_t timeout{kDefaultTimeoutMs};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
