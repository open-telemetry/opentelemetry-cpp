/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

std::unique_ptr<sdklogs::Recordable> OStreamLogExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdklogs::Recordable>(new sdklogs::LogRecord());
}

sdklogs::ExportResult OStreamLogExporter::Export(
    const nostd::span<std::unique_ptr<sdklogs::Recordable>> &records) noexcept
{
  if (is_shutdown_)
  {
    return sdklogs::ExportResult::kFailure;
  }

  for (auto &record : records)
  {
    // Convert recordable to a LogRecord so that the getters of the LogRecord can be used
    auto log_record =
        std::unique_ptr<sdklogs::LogRecord>(static_cast<sdklogs::LogRecord *>(record.release()));

    if (log_record == nullptr)
    {
      // Error "recordable data was lost"
      continue;
    }

    // Convert trace, spanid, traceflags into exportable representation
    constexpr int trace_id_bytes    = 16;
    constexpr int span_id_bytes     = 8;
    constexpr int trace_flags_bytes = 1;

    char trace_id[trace_id_bytes * 2]       = {0};
    char span_id[span_id_bytes * 2]         = {0};
    char trace_flags[trace_flags_bytes * 2] = {0};

    log_record->GetTraceId().ToLowerBase16(trace_id);
    log_record->GetSpanId().ToLowerBase16(span_id);
    log_record->GetTraceFlags().ToLowerBase16(trace_flags);

    // Print out each field of the log record
    sout_ << "{\n"
          << "  timestamp     : " << log_record->GetTimestamp().time_since_epoch().count() << "\n"
          << "  severity_num  : " << static_cast<int>(log_record->GetSeverity()) << "\n"
          << "  severity_text : "
          << opentelemetry::logs::SeverityNumToText[static_cast<int>(log_record->GetSeverity())]
          << "\n"
          << "  name          : " << log_record->GetName() << "\n"
          << "  body          : " << log_record->GetBody() << "\n"
          << "  resource      : {";

    printMap(log_record->GetResource());

    sout_ << "}\n"
          << "  attributes    : {";

    printMap(log_record->GetAttributes());

    sout_ << "}\n"
          << "  trace_id      : " << std::string(trace_id, 32) << "\n"
          << "  span_id       : " << std::string(span_id, 16) << "\n"
          << "  trace_flags   : " << std::string(trace_flags, 2) << "\n"
          << "}\n";
  }

  return sdklogs::ExportResult::kSuccess;
}

bool OStreamLogExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  is_shutdown_ = true;
  return true;
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
