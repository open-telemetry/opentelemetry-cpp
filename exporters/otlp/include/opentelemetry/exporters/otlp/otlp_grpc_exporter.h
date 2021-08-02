// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

#include "opentelemetry/sdk/trace/exporter.h"

#include "opentelemetry/sdk/common/env_variables.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

inline const std::string GetOtlpGrpcDefaultEndpoint()
{
  constexpr char kOtlpGrpcEndpointEnv[]     = "OTEL_EXPORTER_OTLP_GRPC_ENDPOINT";
  constexpr char kOtlpGrpcEndpointDefault[] = "localhost:4317";

  auto endpoint = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpGrpcEndpointEnv);
  return endpoint.size() ? endpoint : kOtlpGrpcEndpointDefault;
}

inline const bool GetOtlpGrpcDefaultIsSslEnable()
{
  constexpr char kOtlpGrpcIsSslEnableEnv[] = "OTEL_EXPORTER_OTLP_GRPC_SSL_ENABLE";
  auto ssl_enable = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpGrpcIsSslEnableEnv);
  if (ssl_enable == "True" || ssl_enable == "TRUE" || ssl_enable == "true" || ssl_enable == "1")
  {
    return true;
  }
  return false;
}

inline const std::string GetOtlpGrpcDefaultSslCertificate()
{
  constexpr char kOtlpGrpcSslCertificate[] = "OTEL_EXPORTER_OTLP_GRPC_SSL_CERTIFICATE";
  auto ssl_cert = opentelemetry::sdk::common::GetEnvironmentVariable(kOtlpGrpcSslCertificate);
  return ssl_cert.size() ? ssl_cert : "";
}

/**
 * Struct to hold OTLP exporter options.
 */
struct OtlpGrpcExporterOptions
{
  // The endpoint to export to. By default the OpenTelemetry Collector's default endpoint.
  std::string endpoint = GetOtlpGrpcDefaultEndpoint();
  // By default when false, uses grpc::InsecureChannelCredentials(); If true,
  // uses ssl_credentials_cacert_path if non-empty, else uses ssl_credentials_cacert_as_string
  bool use_ssl_credentials = GetOtlpGrpcDefaultIsSslEnable();
  // ssl_credentials_cacert_path specifies path to .pem file to be used for SSL encryption.
  std::string ssl_credentials_cacert_path = "";
  // ssl_credentials_cacert_as_string in-memory string representation of .pem file to be used for
  // SSL encryption.
  std::string ssl_credentials_cacert_as_string = GetOtlpGrpcDefaultSslCertificate();
};

/**
 * The OTLP exporter exports span data in OpenTelemetry Protocol (OTLP) format.
 */
class OtlpGrpcExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * Create an OtlpGrpcExporter using all default options.
   */
  OtlpGrpcExporter();

  /**
   * Create an OtlpGrpcExporter using the given options.
   */
  explicit OtlpGrpcExporter(const OtlpGrpcExporterOptions &options);

  /**
   * Create a span recordable.
   * @return a newly initialized Recordable object
   */
  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override;

  /**
   * Export a batch of span recordables in OTLP format.
   * @param spans a span of unique pointers to span recordables
   */
  sdk::common::ExportResult Export(
      const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept override;

  /**
   * Shut down the exporter.
   * @param timeout an optional timeout, the default timeout of 0 means that no
   * timeout is applied.
   * @return return the status of this operation
   */
  bool Shutdown(
      std::chrono::microseconds timeout = std::chrono::microseconds::max()) noexcept override
  {
    return true;
  }

private:
  // The configuration options associated with this exporter.
  const OtlpGrpcExporterOptions options_;

  // For testing
  friend class OtlpGrpcExporterTestPeer;

  // Store service stub internally. Useful for testing.
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> trace_service_stub_;

  /**
   * Create an OtlpGrpcExporter using the specified service stub.
   * Only tests can call this constructor directly.
   * @param stub the service stub to be used for exporting
   */
  OtlpGrpcExporter(std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub);
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
