#include "opentelemetry/exporters/otlp/otlp_exporter.h"
#include "opentelemetry/exporters/otlp/recordable.h"

#include <grpcpp/grpcpp.h>
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// Environment variable to configure endpoint for OTLP exporter
constexpr char kConfiguredEndpoint[] = "OTEL_EXPORTER_OTLP_ENDPOINT";
// Default endpoint is the OpenTelemetry Collector default address
std::string kDefaultEndpoint = "localhost:55678";

// ----------------------------- Helper functions ------------------------------

/**
 * Add span protobufs contained in recordables to request.
 * @param spans the spans to export
 * @param request the current request
 */
void PopulateRequest(const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans,
                     proto::collector::trace::v1::ExportTraceServiceRequest *request)
{
  auto resource_span       = request->add_resource_spans();
  auto instrumentation_lib = resource_span->add_instrumentation_library_spans();

  for (auto &recordable : spans)
  {
    auto rec = std::unique_ptr<Recordable>(static_cast<Recordable *>(recordable.release()));
    *instrumentation_lib->add_spans() = std::move(rec->span());
  }
}

/**
 * Create service stub to communicate with the OpenTelemetry Collector.
 */
std::unique_ptr<proto::collector::trace::v1::TraceService::Stub> MakeServiceStub()
{
  std::string endpoint      = kDefaultEndpoint;
  char *configured_endpoint = getenv(kConfiguredEndpoint);

  if (configured_endpoint != NULL)
  {
    endpoint = configured_endpoint;
  }

  auto channel = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
  return proto::collector::trace::v1::TraceService::NewStub(channel);
}

// -------------------------------- Contructors --------------------------------

OtlpExporter::OtlpExporter() : OtlpExporter(MakeServiceStub()) {}

OtlpExporter::OtlpExporter(
    std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub)
    : trace_service_stub_(std::move(stub))
{}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> OtlpExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new Recordable);
}

sdk::trace::ExportResult OtlpExporter::Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  proto::collector::trace::v1::ExportTraceServiceRequest request;

  PopulateRequest(spans, &request);

  grpc::ClientContext context;
  proto::collector::trace::v1::ExportTraceServiceResponse response;

  grpc::Status status = trace_service_stub_->Export(&context, request, &response);

  if (!status.ok())
  {
    std::cerr << "[OTLP Exporter] Export() failed: " << status.error_message() << "\n";
    return sdk::trace::ExportResult::kFailure;
  }
  return sdk::trace::ExportResult::kSuccess;
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
