// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

// clang-format off

#  include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"
#  include "opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h"
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"

// clang-format on

#  include "opentelemetry/exporters/otlp/otlp_environment.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Struct to hold OTLP metrics exporter options.
 */
struct OtlpGrpcMetricExporterOptions
{

  // The endpoint to export to. By default the OpenTelemetry Collector's default endpoint.
  std::string endpoint = GetOtlpDefaultMetricsEndpoint();
  // By default when false, uses grpc::InsecureChannelCredentials(); If true,
  // uses ssl_credentials_cacert_path if non-empty, else uses ssl_credentials_cacert_as_string
  bool use_ssl_credentials = GetOtlpDefaultMetricsIsSslEnable();
  // ssl_credentials_cacert_path specifies path to .pem file to be used for SSL encryption.
  std::string ssl_credentials_cacert_path = GetOtlpDefaultMetricsSslCertificatePath();
  // ssl_credentials_cacert_as_string in-memory string representation of .pem file to be used for
  // SSL encryption.
  std::string ssl_credentials_cacert_as_string = GetOtlpDefaultMetricsSslCertificateString();
  // Timeout for grpc deadline
  std::chrono::system_clock::duration timeout = GetOtlpDefaultMetricsTimeout();
  // Additional HTTP headers
  OtlpHeaders metadata = GetOtlpDefaultMetricsHeaders();
  opentelemetry::sdk::metrics::AggregationTemporality aggregation_temporality =
      opentelemetry::sdk::metrics::AggregationTemporality::kDelta;
};

/**
 * The OTLP exporter exports metrics data in OpenTelemetry Protocol (OTLP) format in gRPC.
 */
class OtlpGrpcMetricExporter : public opentelemetry::sdk::metrics::MetricExporter
{
public:
  /**
   * Create an OtlpGrpcMetricExporter using all default options.
   */
  OtlpGrpcMetricExporter();

  /**
   * Create an OtlpGrpcMetricExporter using the given options.
   */
  explicit OtlpGrpcMetricExporter(const OtlpGrpcMetricExporterOptions &options);

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept override;

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

private:
  // The configuration options associated with this exporter.
  const OtlpGrpcMetricExporterOptions options_;

  // For testing
  friend class OtlpGrpcExporterTestPeer;

  // Store service stub internally. Useful for testing.
  std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface>
      metrics_service_stub_;

  /**
   * Create an OtlpGrpcMetricExporter using the specified service stub.
   * Only tests can call this constructor directly.
   * @param stub the service stub to be used for exporting
   */
  OtlpGrpcMetricExporter(
      std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub);
  bool is_shutdown_ = false;
  mutable opentelemetry::common::SpinLockMutex lock_;
  bool isShutdown() const noexcept;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif