#include "opentelemetry/exporters/otlp/recordable.h"
#include "opentelemetry/exporters/otlp/shared_utils.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

void Recordable::SetIds(trace::TraceId trace_id,
                        trace::SpanId span_id,
                        trace::SpanId parent_span_id) noexcept
{
  span_.set_trace_id(reinterpret_cast<const char *>(trace_id.Id().data()), trace::TraceId::kSize);
  span_.set_span_id(reinterpret_cast<const char *>(span_id.Id().data()), trace::SpanId::kSize);
  span_.set_parent_span_id(reinterpret_cast<const char *>(parent_span_id.Id().data()),
                           trace::SpanId::kSize);
}

void Recordable::SetAttribute(nostd::string_view key,
                              const opentelemetry::common::AttributeValue &value) noexcept
{
  auto *attribute = span_.add_attributes();
  internal::PopulateAttribute(attribute, key, value);
}

void Recordable::AddEvent(nostd::string_view name,
                          core::SystemTimestamp timestamp,
                          const common::KeyValueIterable &attributes) noexcept
{
  auto *event = span_.add_events();
  event->set_name(name.data(), name.size());
  event->set_time_unix_nano(timestamp.time_since_epoch().count());

  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    internal::PopulateAttribute(event->add_attributes(), key, value);
    return true;
  });
}

void Recordable::AddLink(const opentelemetry::trace::SpanContext &span_context,
                         const common::KeyValueIterable &attributes) noexcept
{
  auto *link = span_.add_links();
  link->set_trace_id(reinterpret_cast<const char *>(span_context.trace_id().Id().data()),
                     trace::TraceId::kSize);
  link->set_span_id(reinterpret_cast<const char *>(span_context.span_id().Id().data()),
                    trace::SpanId::kSize);
  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    internal::PopulateAttribute(link->add_attributes(), key, value);
    return true;
  });

  // TODO: Populate trace_state when it is supported by SpanContext
}

void Recordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept
{
  span_.mutable_status()->set_code(opentelemetry::proto::trace::v1::Status_StatusCode(code));
  span_.mutable_status()->set_message(description.data(), description.size());
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_.set_name(name.data(), name.size());
}

void Recordable::SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept
{
  opentelemetry::proto::trace::v1::Span_SpanKind proto_span_kind =
      opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_UNSPECIFIED;

  switch (span_kind)
  {

    case opentelemetry::trace::SpanKind::kInternal:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_INTERNAL;
      break;

    case opentelemetry::trace::SpanKind::kServer:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_SERVER;
      break;

    case opentelemetry::trace::SpanKind::kClient:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_CLIENT;
      break;

    case opentelemetry::trace::SpanKind::kProducer:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_PRODUCER;
      break;

    case opentelemetry::trace::SpanKind::kConsumer:
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_CONSUMER;
      break;

    default:
      // shouldn't reach here.
      proto_span_kind =
          opentelemetry::proto::trace::v1::Span_SpanKind::Span_SpanKind_SPAN_KIND_UNSPECIFIED;
  }

  span_.set_kind(proto_span_kind);
}

void Recordable::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
  span_.set_start_time_unix_nano(start_time.time_since_epoch().count());
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  const uint64_t unix_end_time = span_.start_time_unix_nano() + duration.count();
  span_.set_end_time_unix_nano(unix_end_time);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
