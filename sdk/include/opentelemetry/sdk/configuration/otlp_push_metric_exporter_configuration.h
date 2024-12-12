// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/push_metric_exporter_configuration_visitor.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

enum enum_default_histogram_aggregation
{
  explicit_bucket_histogram,
  base2_exponential_bucket_histogram
};

// YAML-SCHEMA: schema/meter_provider.json
// YAML-NODE: OtlpMetric
class OtlpPushMetricExporterConfiguration : public PushMetricExporterConfiguration
{
public:
  OtlpPushMetricExporterConfiguration()           = default;
  ~OtlpPushMetricExporterConfiguration() override = default;

  void Accept(PushMetricExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitOtlp(this);
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
  std::string temporality_preference;
  enum_default_histogram_aggregation default_histogram_aggregation{explicit_bucket_histogram};
  bool insecure{false};
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
