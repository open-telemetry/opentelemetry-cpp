#pragma once

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <iostream>

namespace trace    = opentelemetry::trace;
namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

class StdoutExporter final : public sdktrace::SpanExporter
{
  std::unique_ptr<sdktrace::Recordable> MakeRecordable() noexcept
  {
    return std::unique_ptr<sdktrace::Recordable>(new sdktrace::SpanData);
  }

  sdktrace::ExportResult Export(
      const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept
  {
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

        std::cout << "{"
                  << "\n  name          : " << span->GetName()
                  << "\n  trace_id      : " << std::string(trace_id, 32)
                  << "\n  span_id       : " << std::string(span_id, 16)
                  << "\n  parent_span_id: " << std::string(parent_span_id, 16)
                  << "\n  start         : " << span->GetStartTime().time_since_epoch().count()
                  << "\n  duration      : " << span->GetDuration().count() << "\n}"
                  << "\n";
      }
    }

    return sdktrace::ExportResult::kSuccess;
  }

  void Shutdown(std::chrono::microseconds timeout = std::chrono::microseconds(0)) noexcept {}
};
