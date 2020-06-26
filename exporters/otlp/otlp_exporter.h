#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"

#include "opentelemetry/proto/trace/v1/trace.pb.h"

#include "recordable.h"

#include <iostream>

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;
namespace otlpexporter = opentelemetry::exporter::otlp;

class OtlpExporter final : public sdktrace::SpanExporter
{
  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<sdktrace::Recordable>(new opentelemetry::exporter::otlp::Recordable);
  }

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept
  {
    std::cout << "Exporting" << std::endl;
    grpc::ClientContext context;

    opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest request;
    opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse response;

    std::unique_ptr<
      opentelemetry::proto::collector::trace::v1::TraceService::StubInterface>
      trace_service_stub;

    opentelemetry::proto::trace::v1::ResourceSpans resource_span = request.add_resource_spans();


    for (auto &recordable : spans) {
      auto rec = std::unique_ptr<otlpexporter::Recordable>(
          static_cast<otlpexporter::Recordable *>(recordable.release()));

      //std::cout << "Name: " << rec->span().name() << std::endl;

      opentelemetry::proto::trace::v1::Span span = resource_spans.add_spans();
      span->CopyFrom(rec->span());
    }

    //trace_service_stub->Export(&context, request, &response);

    return sdktrace::ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept {}
};
