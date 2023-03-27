// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class PushMetricExporter;
}  // namespace metrics
}  // namespace sdk

namespace exporter
{
namespace otlp
{
struct OtlpGrpcMetricExporterOptions;

/**
 * Factory class for OtlpGrpcMetricExporter.
 */
class OtlpGrpcMetricExporterFactory
{
public:
  /**
   * Create a OtlpGrpcMetricExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Create();

  /**
   * Create a OtlpGrpcMetricExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Create(
      const OtlpGrpcMetricExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
