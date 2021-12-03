// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/span_exporter.h"
#include <iostream>
#include <mutex>
#include "opentelemetry/sdk_config.h"
#include <list>

#include "nlohmann/json.hpp"

namespace nostd     = opentelemetry::nostd;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_api = opentelemetry::trace;
namespace sdkcommon = opentelemetry::sdk::common;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

// This is copied/tweaked from what is in etw/utils.h
/**
 * @brief Convert local system nanoseconds time to ISO8601 string UTC time
 *
 * @param nanoseconds   Nanoseconds since epoch
 *
 * @return ISO8601 UTC string with nanosecond precision
 */
static inline std::string formatUtcTimestampNsAsISO8601(std::chrono::nanoseconds nanoseconds)
{
  char buf[sizeof("YYYY-MM-DDTHH:MM:SS.sssssssssZ") + 1] = {0};
#ifdef _WIN32
  __time64_t seconds = static_cast<__time64_t>(nanoseconds.count() / 1000000000);
  int nano_part   = static_cast<int>(nanoseconds.count() % 1000000000);
  tm tm;
  if (::_gmtime64_s(&tm, &seconds) != 0)
  {
    memset(&tm, 0, sizeof(tm));
  }
  ::_snprintf_s(buf, _TRUNCATE, "%04d-%02d-%02dT%02d:%02d:%02d.%09dZ", 1900 + tm.tm_year,
                1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, nano_part);
#else
  time_t seconds   = static_cast<time_t>(nanoseconds.count() / 1000000000);
  int nano_part = static_cast<int>(nanoseconds.count() % 1000000000);
  tm tm;
  bool valid = (gmtime_r(&seconds, &tm) != NULL);
  if (!valid)
  {
    memset(&tm, 0, sizeof(tm));
  }
  (void)snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%09dZ", 1900 + tm.tm_year,
                 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, nano_part);
#endif
  return buf;
}

std::string formatKind(trace_api::SpanKind span_kind)
{
  switch (span_kind)
  {
    case trace_api::SpanKind::kClient:
      return "Client";
    case trace_api::SpanKind::kInternal:
      return "Internal";
    case trace_api::SpanKind::kServer:
      return "Server";
    case trace_api::SpanKind::kProducer:
      return "Producer";
    case trace_api::SpanKind::kConsumer:
      return "Consumer";
  };
  return "";
}

std::ostream &operator<<(std::ostream &os, trace_api::SpanKind span_kind)
{
  return os << formatKind(span_kind);
}

OStreamSpanExporter::OStreamSpanExporter(std::ostream &sout, bool isJson) noexcept : sout_(sout) { isJson_ = isJson; }

std::unique_ptr<trace_sdk::Recordable> OStreamSpanExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<trace_sdk::Recordable>(new trace_sdk::SpanData);
}

sdk::common::ExportResult OStreamSpanExporter::Export(
    const nostd::span<std::unique_ptr<trace_sdk::Recordable>> &spans) noexcept
{
  if (isShutdown())
  {
    OTEL_INTERNAL_LOG_ERROR("[Ostream Trace Exporter] Exporting "
                            << spans.size() << " span(s) failed, exporter is shutdown");
    return sdk::common::ExportResult::kFailure;
  }

  for (auto &recordable : spans)
  {
    auto span = std::unique_ptr<trace_sdk::SpanData>(
        static_cast<trace_sdk::SpanData *>(recordable.release()));

    if (span != nullptr)
    {
      if (isJson_)
      {
        printSpanJson(span);
      }
      else
      {
        printSpanText(span);
      }
    }
  }

  return sdk::common::ExportResult::kSuccess;
}

void OStreamSpanExporter::printSpanText(const std::unique_ptr<opentelemetry::sdk::trace::SpanData> &span) noexcept
{
  sout_ << "{"
        << "\n  name          : " << span->GetName()
        << "\n  trace_id      : " << formatTraceId(span->GetTraceId())
        << "\n  span_id       : " << formatSpanId(span->GetSpanId())
        << "\n  tracestate    : " << span->GetSpanContext().trace_state()->ToHeader()
        << "\n  parent_span_id: " << formatSpanId(span->GetParentSpanId())
        << "\n  start         : " << span->GetStartTime().time_since_epoch().count()
        << "\n  duration      : " << span->GetDuration().count()
        << "\n  description   : " << span->GetDescription()
        << "\n  span kind     : " << span->GetSpanKind()
        << "\n  status        : " << statusMap[int(span->GetStatus())]
        << "\n  attributes    : ";
  printAttributes(span->GetAttributes());
  sout_ << "\n  events        : ";
  printEvents(span->GetEvents());
  sout_ << "\n  links         : ";
  printLinks(span->GetLinks());
  sout_ << "\n  resources     : ";
  printResources(span->GetResource());
  sout_ << "\n  instr-lib     : ";
  printInstrumentationLibrary(span->GetInstrumentationLibrary());
  sout_ << "\n}\n";
}

bool OStreamSpanExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  is_shutdown_ = true;
  return true;
}

bool OStreamSpanExporter::isShutdown() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return is_shutdown_;
}
void OStreamSpanExporter::printAttributes(
    const std::unordered_map<std::string, sdkcommon::OwnedAttributeValue> &map,
    const std::string prefix)
{
  for (const auto &kv : map)
  {
    sout_ << prefix << kv.first << ": ";
    print_value(kv.second);
  }
}

void OStreamSpanExporter::printEvents(const std::vector<trace_sdk::SpanDataEvent> &events)
{
  for (const auto &event : events)
  {
    sout_ << "\n\t{"
          << "\n\t  name          : " << event.GetName()
          << "\n\t  timestamp     : " << event.GetTimestamp().time_since_epoch().count()
          << "\n\t  attributes    : ";
    printAttributes(event.GetAttributes(), "\n\t\t");
    sout_ << "\n\t}";
  }
}

void OStreamSpanExporter::printLinks(const std::vector<trace_sdk::SpanDataLink> &links)
{
  for (const auto &link : links)
  {
    char trace_id[32] = {0};
    char span_id[16]  = {0};
    link.GetSpanContext().trace_id().ToLowerBase16(trace_id);
    link.GetSpanContext().span_id().ToLowerBase16(span_id);
    sout_ << "\n\t{"
          << "\n\t  trace_id      : " << std::string(trace_id, 32)
          << "\n\t  span_id       : " << std::string(span_id, 16)
          << "\n\t  tracestate    : " << link.GetSpanContext().trace_state()->ToHeader()
          << "\n\t  attributes    : ";
    printAttributes(link.GetAttributes(), "\n\t\t");
    sout_ << "\n\t}";
  }
}

void OStreamSpanExporter::printResources(const opentelemetry::sdk::resource::Resource &resources)
{
  auto attributes = resources.GetAttributes();
  if (attributes.size())
  {
    printAttributes(attributes, "\n\t");
  }
}

void OStreamSpanExporter::printInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library)
{
  sout_ << instrumentation_library.GetName();
  auto version = instrumentation_library.GetVersion();
  if (version.size())
  {
    sout_ << "-" << version;
  }
}

std::string OStreamSpanExporter::formatTraceId(opentelemetry::trace::TraceId trace_id)
{
  char t[2 * trace_api::TraceId::kSize] = {0};

  trace_id.ToLowerBase16(t);

  return std::string(t, 2 * trace_api::TraceId::kSize);
}

std::string OStreamSpanExporter::formatSpanId(opentelemetry::trace::SpanId span_id)
{
  char s[2 * trace_api::SpanId::kSize] = {0};

  span_id.ToLowerBase16(s);

  return std::string(s, 2 * trace_api::SpanId::kSize);
}

void OStreamSpanExporter::printSpanJson(const std::unique_ptr<opentelemetry::sdk::trace::SpanData> &span) noexcept
{
  nlohmann::ordered_json j = nlohmann::ordered_json::object();

  j["name"] = span->GetName();
  j["context"] = formatContext(span->GetSpanContext());
  j["kind"] = formatKind(span->GetSpanKind());

  if (span->GetParentSpanId().IsValid())
  {
    j["parent_id"] = "0x" + formatSpanId(span->GetParentSpanId());
  }

  j["start_time"] = formatUtcTimestampNsAsISO8601(span->GetStartTime().time_since_epoch());
  j["end_time"] = formatUtcTimestampNsAsISO8601(span->GetStartTime().time_since_epoch() + span->GetDuration());

  if (span->GetStatus() != trace_api::StatusCode::kUnset)
  {
    j["status"] = statusMap[int(span->GetStatus())];
    if (span->GetDescription().size() > 0)
    {
      j["description"] = span->GetDescription();
    }
  }

  if (span->GetAttributes().size() > 0)
  {
    j["attributes"] = formatAttributes(span->GetAttributes());
  }

  if (span->GetEvents().size() > 0)
  {
    j["events"] = formatEvents(span->GetEvents());
  }

  if (span->GetLinks().size() > 0)
  {
    j["links"] = formatLinks(span->GetLinks());
  }

  if (span->GetResource().GetAttributes().size() > 0)
  {
    j["resource"] = formatAttributes(span->GetResource().GetAttributes());
  }

  sout_ << j.dump(4, ' ', false, nlohmann::detail::error_handler_t::replace) << "\n";
}

// This is copied/tweaked from what is in otlp_recordable_utils.cc
//
// See `attribute_value.h` for details.
//
const int kOwnedAttributeValueSize = 15;

void OStreamSpanExporter::PopulateAttribute(nostd::string_view key,
                                         const sdk::common::OwnedAttributeValue &value,
                                         nlohmann::basic_json<nlohmann::ordered_map> &attributes)
{
  if (nullptr == attributes)
  {
    return;
  }

  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes
  static_assert(nostd::variant_size<sdk::common::OwnedAttributeValue>::value ==
                    kOwnedAttributeValueSize,
                "OwnedAttributeValue contains unknown type");

  if (nostd::holds_alternative<bool>(value))
  {
    attributes[std::string{key}] = nostd::get<bool>(value);
  }
  else if (nostd::holds_alternative<int32_t>(value))
  {
    attributes[std::string{key}] = nostd::get<int32_t>(value);
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    attributes[std::string{key}] = nostd::get<int64_t>(value);
  }
  else if (nostd::holds_alternative<uint32_t>(value))
  {
    attributes[std::string{key}] = nostd::get<uint32_t>(value);
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    attributes[std::string{key}] = nostd::get<uint64_t>(value);
  }
  else if (nostd::holds_alternative<double>(value))
  {
    attributes[std::string{key}] = nostd::get<double>(value);
  }
  else if (nostd::holds_alternative<std::string>(value))
  {
    attributes[std::string{key}] = nostd::get<std::string>(value);
  }
  /*
  else if (nostd::holds_alternative<std::vector<bool>>(value))
  {
    for (const auto &val : nostd::get<std::vector<bool>>(value))
    {
      ...
    }
  }
  else if (nostd::holds_alternative<std::vector<int32_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<int32_t>>(value))
    {
      ...
    }
  }
  else if (nostd::holds_alternative<std::vector<uint32_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<uint32_t>>(value))
    {
      ...
    }
  }
  else if (nostd::holds_alternative<std::vector<int64_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<int64_t>>(value))
    {
      ...
    }
  }
  else if (nostd::holds_alternative<std::vector<uint64_t>>(value))
  {
    for (const auto &val : nostd::get<std::vector<uint64_t>>(value))
    {
      ...
    }
  }
  else if (nostd::holds_alternative<std::vector<double>>(value))
  {
    for (const auto &val : nostd::get<std::vector<double>>(value))
    {
      ...
    }
  }
  else if (nostd::holds_alternative<std::vector<std::string>>(value))
  {
    for (const auto &val : nostd::get<std::vector<std::string>>(value))
    {
      ...
    }
  }
  */
}

nlohmann::basic_json<nlohmann::ordered_map> OStreamSpanExporter::formatContext(const trace_api::SpanContext &context)
{
  auto c = nlohmann::basic_json<nlohmann::ordered_map>();

  c["trace_id"] = "0x" + formatTraceId(context.trace_id());
  c["span_id"] = "0x" + formatSpanId(context.span_id());
  c["trace_state"] = context.trace_state()->ToHeader();

  return c;
}

nlohmann::basic_json<nlohmann::ordered_map> OStreamSpanExporter::formatAttributes(const std::unordered_map<std::string, sdkcommon::OwnedAttributeValue> &attributes)
{
  auto c = nlohmann::basic_json<nlohmann::ordered_map>();

  for (const auto &kv : attributes)
  {
    PopulateAttribute(kv.first, kv.second, c);
  }

  return c;
}

std::list<nlohmann::basic_json<nlohmann::ordered_map>> OStreamSpanExporter::formatEvents(const std::vector<trace_sdk::SpanDataEvent> &events)
{
  auto json_events = std::list<nlohmann::basic_json<nlohmann::ordered_map>>();

  for (const auto &event : events)
  {
    auto e = nlohmann::basic_json<nlohmann::ordered_map>();

    e["name"] = event.GetName();
    e["timestamp"] = formatUtcTimestampNsAsISO8601(event.GetTimestamp().time_since_epoch());

    if (event.GetAttributes().size() > 0)
    {
      e["attributes"] = formatAttributes(event.GetAttributes());
    }

    json_events.push_back(e);
  }

  return json_events;
}

std::list<nlohmann::basic_json<nlohmann::ordered_map>> OStreamSpanExporter::formatLinks(const std::vector<trace_sdk::SpanDataLink> &links)
{
  auto json_links = std::list<nlohmann::basic_json<nlohmann::ordered_map>>();

  for (const auto &link : links)
  {
    auto l = nlohmann::basic_json<nlohmann::ordered_map>();

    l["context"] = formatContext(link.GetSpanContext());
    if (link.GetAttributes().size() > 0)
    {
      l["attributes"] = formatAttributes(link.GetAttributes());
    }

    json_links.push_back(l);
  }

  return json_links;
}

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
