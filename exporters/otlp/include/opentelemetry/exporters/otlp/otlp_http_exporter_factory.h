// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"
#include "opentelemetry/sdk/trace/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * The OTLP exporter exports span data in OpenTelemetry Protocol (OTLP) format.
 */
class OtlpHttpExporterFactory
{
public:
  /**
   * Create an OtlpHttpExporter using all default options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build();

  /**
   * Create an OtlpHttpExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const OtlpHttpExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
