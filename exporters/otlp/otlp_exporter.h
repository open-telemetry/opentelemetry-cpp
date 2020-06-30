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


OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
class OtlpExporter final : public opentelemetry::sdk::trace::SpanExporter
{
  std::unique_ptr<opentelemetry::sdk::trace::Recordable> MakeRecordable() noexcept override;

  opentelemetry::sdk::trace::ExportResult Export(
    const  opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override {};
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
