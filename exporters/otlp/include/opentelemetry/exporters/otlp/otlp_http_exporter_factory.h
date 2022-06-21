// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

// We need include exporter.h first, which will include Windows.h with NOMINMAX on Windows
#include "opentelemetry/sdk/trace/exporter.h"

#include "opentelemetry/exporters/otlp/otlp_http_exporter_options.h"

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
  static nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build();

  /**
   * Create an OtlpHttpExporter using the given options.
   */
  static nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const OtlpHttpExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
