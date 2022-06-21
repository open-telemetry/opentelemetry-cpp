// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/sdk/trace/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * The OTLP exporter exports span data in OpenTelemetry Protocol (OTLP) format.
 */
class OtlpGrpcExporterFactory
{
public:
  /**
   * Create an OtlpGrpcExporter using all default options.
   */
  static nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build();

  /**
   * Create an OtlpGrpcExporter using the given options.
   */
  static nostd::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const OtlpGrpcExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
