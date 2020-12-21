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
/*********************** Helper functions ************************/

/*
  print_value is used to print out the value of an attribute within a vector.
  These values are held in a variant which makes the process of printing them much more
  complicated.
*/

template <typename T>
void print_value(const T &item, std::ostream &sout)
{
  sout << item;
}

template <typename T>
void print_value(const std::vector<T> &vec, std::ostream &sout)
{
  sout << '[';
  size_t i  = 1;
  size_t sz = vec.size();
  for (auto v : vec)
  {
    sout << v;
    if (i != sz)
      sout << ',' << ' ';
    i++;
  };
  sout << ']';
}

// Prior to C++14, generic lambda is not available so fallback to functor.
#if __cplusplus < 201402L

class OwnedAttributeValueVisitor
{
public:
  OwnedAttributeValueVisitor(std::ostream &sout) : sout_(sout) {}

  template <typename T>
  void operator()(T &&arg)
  {
    print_value(arg, sout_);
  }

private:
  // The OStream to send the logs to
  std::ostream &sout_;
};

#endif

void print_value(sdk::common::OwnedAttributeValue &value, std::ostream &sout)
{
#if __cplusplus < 201402L
  nostd::visit(OwnedAttributeValueVisitor(sout), value);
#else
  nostd::visit([&sout](auto &&arg) { print_value(arg, sout); }, value);
#endif
}

void printMap(std::unordered_map<std::string, sdk::common::OwnedAttributeValue> map,
              std::ostream &sout)
{
  sout << "{";
  size_t size = map.size();
  size_t i    = 1;
  for (auto kv : map)
  {
    sout << "{" << kv.first << ": ";
    print_value(kv.second, sout);
    sout << "}";

    if (i != size)
      sout << ", ";
    i++;
  }
  sout << "}";
}

/*********************** Constructor ***********************/

OStreamLogExporter::OStreamLogExporter(std::ostream &sout) noexcept : sout_(sout) {}

/*********************** Exporter methods ***********************/

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
      // TODO: Log Internal SDK error "recordable data was lost"
      continue;
    }

    // Convert trace, spanid, traceflags into exportable representation
    constexpr int trace_id_len    = 32;
    constexpr int span_id__len    = 16;
    constexpr int trace_flags_len = 2;

    char trace_id[trace_id_len]       = {0};
    char span_id[span_id__len]        = {0};
    char trace_flags[trace_flags_len] = {0};

    log_record->GetTraceId().ToLowerBase16(trace_id);
    log_record->GetSpanId().ToLowerBase16(span_id);
    log_record->GetTraceFlags().ToLowerBase16(trace_flags);

    // Print out each field of the log record, noting that severity is separated
    // into severity_num and severity_text
    sout_ << "{\n"
          << "  timestamp     : " << log_record->GetTimestamp().time_since_epoch().count() << "\n"
          << "  severity_num  : " << static_cast<int>(log_record->GetSeverity()) << "\n"
          << "  severity_text : "
          << opentelemetry::logs::SeverityNumToText[static_cast<int>(log_record->GetSeverity())]
          << "\n"
          << "  name          : " << log_record->GetName() << "\n"
          << "  body          : " << log_record->GetBody() << "\n"
          << "  resource      : ";

    printMap(log_record->GetResource(), sout_);

    sout_ << "\n"
          << "  attributes    : ";

    printMap(log_record->GetAttributes(), sout_);

    sout_ << "\n"
          << "  trace_id      : " << std::string(trace_id, trace_id_len) << "\n"
          << "  span_id       : " << std::string(span_id, span_id__len) << "\n"
          << "  trace_flags   : " << std::string(trace_flags, trace_flags_len) << "\n"
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
