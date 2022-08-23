// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#  include "opentelemetry/sdk/logs/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Factory class for OtlpGrpcLogExporter.
 */
class OtlpGrpcLogExporterFactory
{
public:
  /**
   * Create a OtlpGrpcLogExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogExporter> Create();

  /**
   * Create a OtlpGrpcLogExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogExporter> Create(
      const OtlpGrpcExporterOptions &options);
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE

#endif /* ENABLE_LOGS_PREVIEW */
