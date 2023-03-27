// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class SpanExporter;
}  // namespace trace
}  // namespace sdk

namespace exporter
{
namespace otlp
{

struct OtlpGrpcExporterOptions;

/**
 * Factory class for OtlpGrpcExporter.
 */
class OPENTELEMETRY_EXPORT OtlpGrpcExporterFactory
{
public:
  /**
   * Create an OtlpGrpcExporter using all default options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create();

  /**
   * Create an OtlpGrpcExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(
      const OtlpGrpcExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
