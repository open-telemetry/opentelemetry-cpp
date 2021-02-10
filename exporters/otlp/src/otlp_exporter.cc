#include "opentelemetry/exporters/otlp/otlp_exporter.h"
#include "opentelemetry/exporters/otlp/recordable.h"
#include "opentelemetry/exporters/otlp/shared_utils.h"

#include <grpcpp/grpcpp.h>
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// ----------------------------- Helper functions ------------------------------

void PopulateResource(const sdk::resource::Resource& resource,
                      proto::resource::v1::Resource* proto) {
  // TODO - Fill this out.
  for (const auto& kv : resource.GetAttributes()) {
    internal::PopulateAttribute(proto->add_attributes(), kv.first, kv.second);
  }
  
}

/**
 * Add span protobufs contained in recordables to request.
 * @param spans the spans to export
 * @param request the current request
 */
void PopulateRequest(const sdk::resource::Resource& resource,
                     const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans,
                     proto::collector::trace::v1::ExportTraceServiceRequest *request)
{
  auto resource_span       = request->add_resource_spans();
  PopulateResource(resource, resource_span->mutable_resource());

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
std::unique_ptr<proto::collector::trace::v1::TraceService::Stub> MakeServiceStub(
    std::string endpoint)
{
  auto channel = grpc::CreateChannel(endpoint, grpc::InsecureChannelCredentials());
  return proto::collector::trace::v1::TraceService::NewStub(channel);
}

// -------------------------------- Constructors --------------------------------

OtlpExporter::OtlpExporter() : OtlpExporter(OtlpExporterOptions()) {}

OtlpExporter::OtlpExporter(const OtlpExporterOptions &options)
    : options_(options), trace_service_stub_(MakeServiceStub(options.endpoint))
{}

OtlpExporter::OtlpExporter(
    std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub)
    : options_(OtlpExporterOptions()), trace_service_stub_(std::move(stub))
{}

// ----------------------------- Exporter methods ------------------------------

std::unique_ptr<sdk::trace::Recordable> OtlpExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdk::trace::Recordable>(new Recordable);
}

sdk::trace::ExportResult OtlpExporter::Export(
    const opentelemetry::sdk::resource::Resource& resource,
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  proto::collector::trace::v1::ExportTraceServiceRequest request;
  PopulateRequest(resource, spans, &request);

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
