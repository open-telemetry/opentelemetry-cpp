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

struct OtlpHttpExporterOptions;

/**
 * Factory class for OtlpHttpExporter.
 */
class OPENTELEMETRY_EXPORT OtlpHttpExporterFactory
{
public:
  /**
   * Create an OtlpHttpExporter using all default options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create();

  /**
   * Create an OtlpHttpExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(
      const OtlpHttpExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
