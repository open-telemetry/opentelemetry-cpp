#include "exporters/trace/stream/stream.h"

#include <iostream>

namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{
StreamSpanExporter::StreamSpanExporter(std::ostream &sout,
                                      bool isShutdown) noexcept
                                    : sout_{sout},
                                      isShutdown_{isShutdown} {}

std::unique_ptr<sdktrace::Recordable> StreamSpanExporter::MakeRecordable() noexcept 
{
    return std::unique_ptr<sdktrace::Recordable>(new sdktrace::SpanData);
}

sdktrace::ExportResult StreamSpanExporter::Export(
    const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept
{
    if(isShutdown_)
    {
        return sdktrace::ExportResult::kFailure;
    }

    for (auto &recordable : spans)
    {
      auto span = std::unique_ptr<sdktrace::SpanData>(
          static_cast<sdktrace::SpanData *>(recordable.release()));

      if (span != nullptr)
      {

        char trace_id[32]       = {0};
        char span_id[16]        = {0};
        char parent_span_id[16] = {0};

        span->GetTraceId().ToLowerBase16(trace_id);
        span->GetSpanId().ToLowerBase16(span_id);
        span->GetParentSpanId().ToLowerBase16(parent_span_id);

        sout_ << "{"
              << "\n  name          : " << span->GetName()
              << "\n  trace_id      : " << std::string(trace_id, 32)
              << "\n  span_id       : " << std::string(span_id, 16)
              << "\n  parent_span_id: " << std::string(parent_span_id, 16)
              << "\n  start         : " << span->GetStartTime().time_since_epoch().count()
              << "\n  duration      : " << span->GetDuration().count()
              << "\n  description   : " << span->GetDescription() 
              << "\n  status        : " << statusMap[int(span->GetStatus())]
              << "\n  attributes    : ";
              printAttributes(span->GetAttributes());
      }
    }

    return sdktrace::ExportResult::kSuccess;
}

void StreamSpanExporter::Shutdown(std::chrono::microseconds timeout) noexcept 
{
  isShutdown_ = true;
}

} // namespace trace
} // namespace exporter
OPENTELEMETRY_END_NAMESPACE
