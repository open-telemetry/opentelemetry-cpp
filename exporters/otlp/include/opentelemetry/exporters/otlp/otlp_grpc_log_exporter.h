// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"
#  include "opentelemetry/proto/collector/logs/v1/logs_service.grpc.pb.h"
#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_environment.h"
#  include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"
#  include "opentelemetry/exporters/otlp/otlp_recordable.h"
#  include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// /**
//  * Struct to hold OTLP log exporter options.
//  */
// struct OtlpGrpcLogExporterOptions
// {
//     std::string url = GetOtlpDefaultGrpcEndpoint();
// };

/**
 * The OTLP exporter exports log data in OpenTelemetry Protocol (OTLP) format in gRPC.
 */
class OtlpGrpcLogExporter : public opentelemetry::sdk::logs::LogExporter
{
  /**
   * Create an OtlpGrpcLogExporter with default exporter options.
   */
  OtlpGrpcLogExporter();

  /**
   * Create an OtlpGrpcLogExporter with user specified options.
   * @param options An object containing the user's configuration options.
   */
  OtlpGrpcLogExporter(const OtlpGrpcExporterOptions &options);

  /**
   * Creates a recordable that stores the data in protobuf.
   * @return a newly initialized Recordable object.
   */
  std::unique_ptr<opentelemetry::sdk::logs::Recordable> MakeRecordable() noexcept override;

  /**
   * Exports a vector of log records to the configured gRPC endpoint. Guaranteed to return after a
   * timeout specified from the options passed to the constructor.
   * @param records A list of log records.
   */
  opentelemetry::sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &records) noexcept
      override;

  /**
   * Shutdown this exporter.
   * @param timeout The maximum time to wait for the shutdown method to return.
   */
  bool Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override
  {
      return true;
  }

private:

  // Configuration options for the exporter
  const OtlpGrpcExporterOptions options_;

  // For testing
  friend class OtlpGrpcExporterTestPeer;

  // Store service stub internally. Useful for testing.
  std::unique_ptr<proto::collector::logs::v1::LogService::StubInterface> log_service_stub_;

  /**
   * Create an OtlpGrpcLogExporter using the specified service stub.
   * Only tests can call this constructor directly.
   * @param stub the service stub to be used for exporting
   */
  OtlpGrpcExporter(std::unique_ptr<proto::collector::logs::v1::LogService::StubInterface> stub);
};

}  // namespace otlp
}  // namespace exporter

#endif
