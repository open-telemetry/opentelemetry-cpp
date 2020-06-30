#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "src/common/random.h"

#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"

#include "opentelemetry/proto/trace/v1/trace.pb.h"

#include "recordable.h"

#include <grpcpp/grpcpp.h>

#include <iostream>

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;
namespace otlpexporter = opentelemetry::exporter::otlp;
namespace protocollector = opentelemetry::proto::collector::trace::v1;
namespace prototrace = opentelemetry::proto::trace::v1;

trace::TraceId GenerateRandomTraceId()
{
    uint8_t trace_id_buf[trace::TraceId::kSize];
    opentelemetry::sdk::common::Random::GenerateRandomBuffer(trace_id_buf);
    return trace::TraceId(trace_id_buf);
}

trace::SpanId GenerateRandomSpanId()
{
    uint8_t span_id_buf[trace::SpanId::kSize];
    opentelemetry::sdk::common::Random::GenerateRandomBuffer(span_id_buf);
    return trace::SpanId(span_id_buf);
}

class OtlpExporter final : public sdktrace::SpanExporter
{
  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<sdktrace::Recordable>(new otlpexporter::Recordable);
  }

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept
  {
    std::cout << "Exporting" << std::endl;
    grpc::ClientContext context;

    protocollector::ExportTraceServiceRequest request;
    protocollector::ExportTraceServiceResponse response;

    std::unique_ptr<
      protocollector::TraceService::StubInterface>
      trace_service_stub;
    
    const std::string address = "localhost:55678";
    auto channel = grpc::CreateChannel(
        address, grpc::InsecureChannelCredentials());
    
    trace_service_stub = protocollector::TraceService::NewStub(channel);

    prototrace::ResourceSpans* resource_span = request.add_resource_spans();
    prototrace::InstrumentationLibrarySpans* instrumentation_lib = resource_span->add_instrumentation_library_spans();

    for (auto &recordable : spans) {

      auto rec = std::unique_ptr<otlpexporter::Recordable>(
          static_cast<otlpexporter::Recordable *>(recordable.release()));

      std::cout << "Name: " << rec->span().name() << std::endl;
      // std::cout << "Start time: " << rec->span().start_time_unixnano() << std::endl;
      // std::cout << "End time: " << rec->span().end_time_unixnano() << std::endl;

      // Temporarily set IDs here until Span is updated to do it
      rec->SetIds(GenerateRandomTraceId(), GenerateRandomSpanId(), GenerateRandomSpanId());

      // std::cout << "Trace ID: " << rec->span().trace_id() << std::endl;
      // std::cout << "Span ID: " << rec->span().span_id() << std::endl;
      // std::cout << "Parent span ID: " << rec->span().parent_span_id() << std::endl;

      prototrace::Span* span = instrumentation_lib->add_spans();

      span->CopyFrom(rec->span());
    }

     grpc::Status status = trace_service_stub->Export(&context, request, &response);

    if(status.ok()){
      std::cout << "Status OK" << std::endl;
      return sdktrace::ExportResult::kSuccess;
    }
    std::cout << "ERROR " << status.error_code() << ": " << status.error_message()
              << std::endl;
    return sdktrace::ExportResult::kFailure;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept {}
};
