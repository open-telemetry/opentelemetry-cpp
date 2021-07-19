// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/ostream/span_exporter.h"

#include <iostream>

namespace nostd    = opentelemetry::nostd;
namespace sdktrace = opentelemetry::sdk::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

std::ostream &operator<<(std::ostream &os, opentelemetry::trace::SpanKind span_kind)
{
  switch (span_kind)
  {
    case opentelemetry::trace::SpanKind::kClient:
      return os << "Client";
    case opentelemetry::trace::SpanKind::kInternal:
      return os << "Internal";
    case opentelemetry::trace::SpanKind::kServer:
      return os << "Server";
    case opentelemetry::trace::SpanKind::kProducer:
      return os << "Producer";
    case opentelemetry::trace::SpanKind::kConsumer:
      return os << "Consumer";
  };
  return os << "";
}

OStreamSpanExporter::OStreamSpanExporter(std::ostream &sout) noexcept : sout_(sout) {}

std::unique_ptr<sdktrace::Recordable> OStreamSpanExporter::MakeRecordable() noexcept
{
  return std::unique_ptr<sdktrace::Recordable>(new sdktrace::SpanData);
}

sdk::common::ExportResult OStreamSpanExporter::Export(
    const nostd::span<std::unique_ptr<sdktrace::Recordable>> &spans) noexcept
{
  if (isShutdown_)
  {
    return sdk::common::ExportResult::kFailure;
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
            << "\n  tracestate    : " << span->GetSpanContext().trace_state()->ToHeader()
            << "\n  parent_span_id: " << std::string(parent_span_id, 16)
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
  }

  return sdk::common::ExportResult::kSuccess;
}

bool OStreamSpanExporter::Shutdown(std::chrono::microseconds timeout) noexcept
{
  isShutdown_ = true;
  return true;
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

void OStreamSpanExporter::printEvents(const std::vector<sdktrace::SpanDataEvent> &events)
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

void OStreamSpanExporter::printLinks(const std::vector<sdktrace::SpanDataLink> &links)
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

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
