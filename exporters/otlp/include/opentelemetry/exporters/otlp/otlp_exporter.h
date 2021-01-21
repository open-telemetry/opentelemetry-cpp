#pragma once

#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
#include "opentelemetry/sdk/trace/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
/**
 * Struct to hold OTLP exporter options.
 */
struct OtlpExporterOptions
{
  // The endpoint to export to. By default the OpenTelemetry Collector's default endpoint.
  std::string endpoint = "localhost:4317";
};

/**
 * The OTLP exporter exports span data in OpenTelemetry Protocol (OTLP) format.
 */
class OtlpExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  /**
   * Create an OtlpExporter using all default options.
   */
  OtlpExporter();

  /**
   * Create an OtlpExporter using the given options.
   */
  OtlpExporter(const OtlpExporterOptions &options);

  /**
   * Create a span recordable.
   * @return a newly initialized Recordable object
   */
  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override;

  /**
   * Export a batch of span recordables in OTLP format.
   * @param spans a span of unique pointers to span recordables
   */
  sdk::trace::ExportResult Export(
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
  const OtlpExporterOptions options_;

  // For testing
  friend class OtlpExporterTestPeer;

  // Store service stub internally. Useful for testing.
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> trace_service_stub_;

  /**
   * Create an OtlpExporter using the specified service stub.
   * Only tests can call this constructor directly.
   * @param stub the service stub to be used for exporting
   */
  OtlpExporter(std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub);
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
