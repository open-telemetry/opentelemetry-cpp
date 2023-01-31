// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef OPENTELEMETRY_NO_DEPRECATED_CODE
#  error "header <opentelemetry/exporters/jaeger/jaeger_exporter_factory.h> is deprecated."
#endif

#include <opentelemetry/exporters/jaeger/jaeger_exporter_options.h>
#include <opentelemetry/sdk/trace/exporter.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

/**
 * Factory class for JaegerExporter.
 */
class OPENTELEMETRY_DEPRECATED JaegerExporterFactory
{
public:
  /**
   * Create a JaegerExporter using all default options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create();

  /**
   * Create a JaegerExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(
      const JaegerExporterOptions &options);
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
