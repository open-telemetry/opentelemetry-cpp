// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"
// clang-format on

#include "opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"
// clang-format on

#include "opentelemetry/exporters/otlp/otlp_environment.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"

#include <atomic>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

#ifdef ENABLE_ASYNC_EXPORT
class OtlpGrpcClientReferenceGuard;
#endif
class OtlpGrpcClient;

/**
 * The OTLP exporter exports metrics data in OpenTelemetry Protocol (OTLP) format in gRPC.
 */
class OtlpGrpcMetricExporter : public opentelemetry::sdk::metrics::PushMetricExporter
{
public:
  /**
   * Create an OtlpGrpcMetricExporter using all default options.
   */
  OtlpGrpcMetricExporter();

#ifdef ENABLE_ASYNC_EXPORT
  /**
   * Create an OtlpGrpcMetricExporter using specified OtlpGrpcClient.
   *
   * @param options options to create exporter
   * @param client the gRPC client to use
   */
  OtlpGrpcMetricExporter(const OtlpGrpcMetricExporterOptions &options,
                         nostd::shared_ptr<OtlpGrpcClient> client);
#endif

  /**
   * Create an OtlpGrpcMetricExporter using the given options.
   */
  explicit OtlpGrpcMetricExporter(const OtlpGrpcMetricExporterOptions &options);

  ~OtlpGrpcMetricExporter() override;

  /**
   * Get the AggregationTemporality for exporter
   *
   * @return AggregationTemporality
   */
  sdk::metrics::AggregationTemporality GetAggregationTemporality(
      sdk::metrics::InstrumentType instrument_type) const noexcept override;

  opentelemetry::sdk::common::ExportResult Export(
      const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept override;

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

#ifdef ENABLE_ASYNC_EXPORT
  /**
   * Get the Client object
   *
   * @return return binded gRPC client
   */
  const nostd::shared_ptr<OtlpGrpcClient> &GetClient() const noexcept;
#endif

private:
  // The configuration options associated with this exporter.
  const OtlpGrpcMetricExporterOptions options_;

#ifdef ENABLE_ASYNC_EXPORT
  nostd::shared_ptr<OtlpGrpcClient> client_;
  nostd::shared_ptr<OtlpGrpcClientReferenceGuard> client_reference_guard_;
#endif

  // Aggregation Temporality selector
  const sdk::metrics::AggregationTemporalitySelector aggregation_temporality_selector_;

  // For testing
  friend class OtlpGrpcMetricExporterTestPeer;

  // Store service stub internally. Useful for testing.
  nostd::shared_ptr<proto::collector::metrics::v1::MetricsService::StubInterface>
      metrics_service_stub_;

  /**
   * Create an OtlpGrpcMetricExporter using the specified service stub.
   * Only tests can call this constructor directly.
   * @param stub the service stub to be used for exporting
   */
  OtlpGrpcMetricExporter(
      std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub);
#ifdef ENABLE_ASYNC_EXPORT
  /**
   * Create an OtlpGrpcMetricExporter using the specified service stub and gRPC client.
   * Only tests can call this constructor directly.
   * @param stub the service stub to be used for exporting
   * @param client the gRPC client to use
   */
  OtlpGrpcMetricExporter(
      std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub,
      nostd::shared_ptr<OtlpGrpcClient> client);
#endif

  std::atomic<bool> is_shutdown_{false};
  bool isShutdown() const noexcept;
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
