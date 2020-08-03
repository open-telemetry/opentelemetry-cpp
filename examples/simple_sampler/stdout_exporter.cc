#include "stdout_exporter.h"

#include <iostream>

namespace example
{
std::unique_ptr<sdktrace::Recordable> StdoutExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdktrace::Recordable>(new sdktrace::SpanData);
}
sdktrace::ExportResult StdoutExporter::Export(
    const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept
{
  for (auto &recordable : spans)
  {
    const auto* span = static_cast<const sdktrace::SpanData*>(recordable.get());

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
}  // namespace example