// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/exporter_default_histogram_aggregation.h"
#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/otlp_http_encoding.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

// YAML-SCHEMA: schema/meter_provider.json
// YAML-NODE: OtlpHttpMetricExporter
class OtlpHttpPushMetricExporterConfiguration : public PushMetricExporterConfiguration
{
public:
  OtlpHttpPushMetricExporterConfiguration()           = default;
  ~OtlpHttpPushMetricExporterConfiguration() override = default;

  void Accept(PushMetricExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitOtlpHttp(this);
  }

  std::string endpoint;
  std::string certificate_file;
  std::string client_key_file;
  std::string client_certificate_file;
  std::unique_ptr<HeadersConfiguration> headers;
  std::string headers_list;
  std::string compression;
  size_t timeout{0};
  enum_otlp_http_encoding encoding{protobuf};
  std::string temporality_preference;
  enum_default_histogram_aggregation default_histogram_aggregation{explicit_bucket_histogram};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
