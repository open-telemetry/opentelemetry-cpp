// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/logs/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

class OtlpGrpcClient;

/**
 * Factory class for OtlpGrpcLogRecordExporter.
 */
class OPENTELEMETRY_EXPORT OtlpGrpcLogRecordExporterFactory
{
public:
  /**
   * Create a OtlpGrpcLogRecordExporter.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create();

  /**
   * Create a OtlpGrpcLogRecordExporter using the given options.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
      const OtlpGrpcLogRecordExporterOptions &options);

#ifdef ENABLE_ASYNC_EXPORT
  /**
   * Create a OtlpGrpcLogRecordExporter using the given options and gRPC client.
   */
  static std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Create(
      const OtlpGrpcLogRecordExporterOptions &options,
      nostd::shared_ptr<OtlpGrpcClient> client);
#endif
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
