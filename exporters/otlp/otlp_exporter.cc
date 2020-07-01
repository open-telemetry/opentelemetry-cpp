#include "otlp_exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

std::unique_ptr<sdk::trace::Recordable> OtlpExporter::MakeRecordable() noexcept
{
return std::unique_ptr<sdk::trace::Recordable>(new Recordable);
}

sdk::trace::ExportResult OtlpExporter::Export(
  const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept
{
  std::cout << "Exporting" << std::endl;
  grpc::ClientContext context;

  proto::collector::trace::v1::ExportTraceServiceRequest request;
  proto::collector::trace::v1::ExportTraceServiceResponse response;

  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> trace_service_stub;

  const std::string address = "localhost:55678";
  auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());

  trace_service_stub = proto::collector::trace::v1::TraceService::NewStub(channel);

  proto::trace::v1::ResourceSpans* resource_span = request.add_resource_spans();
  proto::trace::v1::InstrumentationLibrarySpans* instrumentation_lib = resource_span->add_instrumentation_library_spans();

  for (auto &recordable : spans) {

    auto rec = std::unique_ptr<Recordable>(
        static_cast<Recordable *>(recordable.release()));

    std::cout << "Name: " << rec->span().name() << std::endl;

    proto::trace::v1::Span* span = instrumentation_lib->add_spans();

    span->CopyFrom(rec->span());
  }

  grpc::Status status = trace_service_stub->Export(&context, request, &response);

  if(status.ok()){
    std::cout << "Status OK" << std::endl;
    return sdk::trace::ExportResult::kSuccess;
  }
  std::cout << "ERROR " << status.error_code() << ": " << status.error_message()
            << std::endl;
  return sdk::trace::ExportResult::kFailure;
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
