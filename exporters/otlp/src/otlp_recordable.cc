// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_recordable.h"

#include "opentelemetry/exporters/otlp/otlp_recordable_utils.h"

namespace nostd = opentelemetry::nostd;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void OtlpRecordable::SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                                 opentelemetry::trace::SpanId parent_span_id) noexcept
{
  span_.set_trace_id(reinterpret_cast<const char *>(span_context.trace_id().Id().data()),
                     trace::TraceId::kSize);
  span_.set_span_id(reinterpret_cast<const char *>(span_context.span_id().Id().data()),
                    trace::SpanId::kSize);
  if (parent_span_id.IsValid())
  {
    span_.set_parent_span_id(reinterpret_cast<const char *>(parent_span_id.Id().data()),
                             trace::SpanId::kSize);
  }
  span_.set_trace_state(span_context.trace_state()->ToHeader());
}

proto::resource::v1::Resource OtlpRecordable::ProtoResource() const noexcept
{
  proto::resource::v1::Resource proto;
  if (resource_)
  {
    OtlpRecordableUtils::PopulateAttribute(&proto, *resource_);
  }

  return proto;
}

const std::string OtlpRecordable::GetResourceSchemaURL() const noexcept
{
  std::string schema_url;
  if (resource_)
  {
    schema_url = resource_->GetSchemaURL();
  }

  return schema_url;
}

const std::string OtlpRecordable::GetInstrumentationLibrarySchemaURL() const noexcept
{
  std::string schema_url;
  if (instrumentation_library_)
  {
    schema_url = instrumentation_library_->GetSchemaURL();
  }

  return schema_url;
}

proto::common::v1::InstrumentationLibrary OtlpRecordable::GetProtoInstrumentationLibrary()
    const noexcept
{
  proto::common::v1::InstrumentationLibrary instrumentation_library;
  if (instrumentation_library_)
  {
    instrumentation_library.set_name(instrumentation_library_->GetName());
    instrumentation_library.set_version(instrumentation_library_->GetVersion());
  }
  return instrumentation_library;
}

void OtlpRecordable::SetResource(const sdk::resource::Resource &resource) noexcept
{
  resource_ = &resource;
};

void OtlpRecordable::SetAttribute(nostd::string_view key,
                                  const common::AttributeValue &value) noexcept
{
  auto *attribute = span_.add_attributes();
  OtlpRecordableUtils::PopulateAttribute(attribute, key, value);
}

void OtlpRecordable::AddEvent(nostd::string_view name,
                              common::SystemTimestamp timestamp,
                              const common::KeyValueIterable &attributes) noexcept
{
  auto *event = span_.add_events();
  event->set_name(name.data(), name.size());
  event->set_time_unix_nano(timestamp.time_since_epoch().count());

  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    OtlpRecordableUtils::PopulateAttribute(event->add_attributes(), key, value);
    return true;
  });
}

void OtlpRecordable::AddLink(const trace::SpanContext &span_context,
                             const common::KeyValueIterable &attributes) noexcept
{
  auto *link = span_.add_links();
  link->set_trace_id(reinterpret_cast<const char *>(span_context.trace_id().Id().data()),
                     trace::TraceId::kSize);
  link->set_span_id(reinterpret_cast<const char *>(span_context.span_id().Id().data()),
                    trace::SpanId::kSize);
  link->set_trace_state(span_context.trace_state()->ToHeader());
  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    OtlpRecordableUtils::PopulateAttribute(link->add_attributes(), key, value);
    return true;
  });
}

void OtlpRecordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept
{
  span_.mutable_status()->set_code(proto::trace::v1::Status_StatusCode(code));
  if (code == trace::StatusCode::kError)
  {
    span_.mutable_status()->set_message(description.data(), description.size());
  }
}

void OtlpRecordable::SetName(nostd::string_view name) noexcept
{
  span_.set_name(name.data(), name.size());
}

void OtlpRecordable::SetSpanKind(trace::SpanKind span_kind) noexcept
{
  proto::trace::v1::Span_SpanKind proto_span_kind =
      proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_UNSPECIFIED;

  switch (span_kind)
  {

    case trace::SpanKind::kInternal:
      proto_span_kind = proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_INTERNAL;
      break;

    case trace::SpanKind::kServer:
      proto_span_kind = proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_SERVER;
      break;

    case trace::SpanKind::kClient:
      proto_span_kind = proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_CLIENT;
      break;

    case trace::SpanKind::kProducer:
      proto_span_kind = proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_PRODUCER;
      break;

    case trace::SpanKind::kConsumer:
      proto_span_kind = proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_CONSUMER;
      break;

    default:
      // shouldn't reach here.
      proto_span_kind = proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_UNSPECIFIED;
  }

  span_.set_kind(proto_span_kind);
}

void OtlpRecordable::SetStartTime(common::SystemTimestamp start_time) noexcept
{
  span_.set_start_time_unix_nano(start_time.time_since_epoch().count());
}

void OtlpRecordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  const uint64_t unix_end_time = span_.start_time_unix_nano() + duration.count();
  span_.set_end_time_unix_nano(unix_end_time);
}

void OtlpRecordable::SetInstrumentationLibrary(
    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
        &instrumentation_library) noexcept
{
  instrumentation_library_ = &instrumentation_library;
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
