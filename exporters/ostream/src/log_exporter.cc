#include "opentelemetry/exporters/ostream/log_exporter.h"

#include <iostream>

namespace nostd   = opentelemetry::nostd;
namespace sdklogs = opentelemetry::sdk::logs;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
OStreamLogExporter::OStreamLogExporter(std::ostream &sout) noexcept : sout_(sout) {}

sdklogs::ExportResult OStreamLogExporter::Export(
    const nostd::span<std::shared_ptr<opentelemetry::logs::LogRecord>> &records) noexcept
{
  if (isShutdown_)
  {
    return sdklogs::ExportResult::kFailure;
  }

  for (auto &record : records)
  {
    // Convert trace, spanid, traceflags into string convertable representation
    constexpr int trace_id_bytes      = 16;
    char trace_id[trace_id_bytes * 2] = {0};
    record->trace_id.ToLowerBase16(trace_id);

    constexpr int span_id_bytes     = 8;
    char span_id[span_id_bytes * 2] = {0};
    record->span_id.ToLowerBase16(span_id);

    constexpr int trace_flags_bytes         = 1;
    char trace_flags[trace_flags_bytes * 2] = {0};
    record->trace_flags.ToLowerBase16(trace_flags);

    /*** Print out each field of the log record ***/

    // Print fields most useful to user first
    sout_ << "{\n"
          << "    timestamp   : " << record->timestamp.time_since_epoch().count() << "\n"
          << "    severity    : " << severityNumToText[static_cast<int>(record->severity)] << "\n"
          << "    name        : " << record->name << "\n"
          << "    body        : " << record->body << "\n";

    // Print "resource" field
    sout_ << "    resource    : {";
    bool firstKV = true;
    if (record->resource != nullptr)
    {
      record->resource->ForEachKeyValue([&](nostd::string_view key,
                                            common::AttributeValue value) noexcept {
        printKV(firstKV, key, value);
        return true;
      });
    }
    sout_ << "}\n";

    // Print "attributes" field
    sout_ << "    attributes  : {";
    firstKV = true;
    if (record->attributes != nullptr)
    {
      firstKV = true;
      record->attributes->ForEachKeyValue([&](nostd::string_view key,
                                              common::AttributeValue value) noexcept {
        printKV(firstKV, key, value);
        return true;
      });
    }
    sout_ << "}\n";

    // Print span context fields
    sout_ << "    trace_id    : " << std::string(trace_id, 32) << "\n"
          << "    span_id     : " << std::string(span_id, 16) << "\n"
          << "    trace_flags : " << std::string(trace_flags, 2) << "\n"
          << "}\n";
  }

  return sdklogs::ExportResult::kSuccess;
}

bool OStreamLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  isShutdown_ = true;
  return true;
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
