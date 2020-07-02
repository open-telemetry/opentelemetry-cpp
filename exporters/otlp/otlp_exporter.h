#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"
#include "src/common/random.h"
#include "recordable.h"

#include <grpcpp/grpcpp.h>
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
class OtlpExporter final : public opentelemetry::sdk::trace::SpanExporter
{
public:
  OtlpExporter();

  std::unique_ptr<sdk::trace::Recordable> MakeRecordable() noexcept override;

  sdk::trace::ExportResult Export(
    const nostd::span<std::unique_ptr<sdk::trace::Recordable>> &spans) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override {};

private:
  // For testing
  friend class OtlpExporterTestPeer;

  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> trace_service_stub_;

  OtlpExporter(std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub);
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
