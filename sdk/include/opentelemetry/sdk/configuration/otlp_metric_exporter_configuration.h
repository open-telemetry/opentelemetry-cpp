// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/sdk/configuration/headers_configuration.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration.h"
#include "opentelemetry/sdk/configuration/metric_exporter_configuration_visitor.h"
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

class OtlpMetricExporterConfiguration : public MetricExporterConfiguration
{
public:
  OtlpMetricExporterConfiguration()           = default;
  ~OtlpMetricExporterConfiguration() override = default;

  void Accept(MetricExporterConfigurationVisitor *visitor) const override
  {
    visitor->VisitOtlp(this);
  }

  std::string protocol;
  std::string endpoint;
  std::string certificate;
  std::string client_key;
  std::string client_certificate;
  std::unique_ptr<HeadersConfiguration> headers;
  std::string compression;
  size_t timeout;
  std::string temporality_preference;
  enum_default_histogram_aggregation default_histogram_aggregation;
  bool insecure;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
