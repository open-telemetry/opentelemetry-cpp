// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <mutex>

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_metric_utils.h"

#include "opentelemetry/sdk_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
// -------------------------------- Constructors --------------------------------

OtlpGrpcMetricExporter::OtlpGrpcMetricExporter()
    : OtlpGrpcMetricExporter(OtlpGrpcMetricExporterOptions())
{}

OtlpGrpcMetricExporter::OtlpGrpcMetricExporter(const OtlpGrpcMetricExporterOptions &options)
    : options_(options),
      aggregation_temporality_selector_{
          OtlpMetricUtils::ChooseTemporalitySelector(options_.aggregation_temporality)},
      metrics_service_stub_(OtlpGrpcClient::MakeMetricsServiceStub(options))
{}

OtlpGrpcMetricExporter::OtlpGrpcMetricExporter(
    std::unique_ptr<proto::collector::metrics::v1::MetricsService::StubInterface> stub)
    : options_(OtlpGrpcMetricExporterOptions()),
      aggregation_temporality_selector_{
          OtlpMetricUtils::ChooseTemporalitySelector(options_.aggregation_temporality)},
      metrics_service_stub_(std::move(stub))
{}

// ----------------------------- Exporter methods ------------------------------

sdk::metrics::AggregationTemporality OtlpGrpcMetricExporter::GetAggregationTemporality(
    sdk::metrics::InstrumentType instrument_type) const noexcept
{
  return aggregation_temporality_selector_(instrument_type);
}

opentelemetry::sdk::common::ExportResult OtlpGrpcMetricExporter::Export(
    const opentelemetry::sdk::metrics::ResourceMetrics &data) noexcept
{

  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[OTLP METRICS gRPC] Exporting "
                            << data.scope_metric_data_.size()
                            << " metric(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }
  if (data.scope_metric_data_.empty())
  {
    return sdk::common::ExportResult::kSuccess;
  }

  proto::collector::metrics::v1::ExportMetricsServiceRequest request;
  OtlpMetricUtils::PopulateRequest(data, &request);

  auto context = OtlpGrpcClient::MakeClientContext(options_);
  proto::collector::metrics::v1::ExportMetricsServiceResponse response;

  grpc::Status status = OtlpGrpcClient::DelegateExport(metrics_service_stub_.get(), context.get(),
                                                       request, &response);

  if (!status.ok())
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[OTLP METRIC GRPC Exporter] Export() failed: " << status.error_message());
    return sdk::common::ExportResult::kFailure;
  }
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool OtlpGrpcMetricExporter::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  // TODO: OTLP gRPC exporter does not support concurrency exporting now.
  return true;
}

bool OtlpGrpcMetricExporter::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool OtlpGrpcMetricExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
