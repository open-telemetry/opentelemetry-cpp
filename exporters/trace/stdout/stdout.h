#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <iostream>
#include <chrono>
#include <map>

namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{
class StdoutSpanExporter final : public sdktrace::SpanExporter
{
public:
  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept override;

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept override;

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept override;

private:
  bool isShutdown_ = false;
  std::map<int, std::string> statusMap {
    {0, "OK"},
    {1, "CANCELLED"},
    {2, "UNKNOWN"},
    {3, "INVALID_ARGUMENT"},
    {4, "DEADLINE_EXCEEDED"},
    {5, "NOT_FOUND"},
    {6, "ALREADY_EXISTS"},
    {7, "PERMISSION_DENIED"},
    {8, "RESOURCE_EXHAUSTED"},
    {9, "FAILED_PRECONDITION"},
    {10,"ABORTED"},
    {11,"OUT_OF_RANGE"},
    {12,"UNIMPLEMENTED"},
    {13,"INTERNAL"},
    {14,"UNAVAILABLE"},
    {15,"DATA_LOSS"},
    {16,"UNAUTHENTICATED"}
  };
};
}
}
OPENTELEMETRY_END_NAMESPACE