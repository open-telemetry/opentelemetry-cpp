#include "exporters/otlp/recordable.h"

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
                              const opentelemetry::common::AttributeValue &&value) noexcept
{
  (void)key;
  (void)value;
}

void Recordable::AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
{
  (void)name;
}

void Recordable::SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept
{
  (void)code;
  (void)description;
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_.set_name(name.data(), name.size());
}

void Recordable::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
  const uint64_t nano_unix_time = start_time.time_since_epoch().count();
  span_.set_start_time_unix_nano(nano_unix_time);
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  const uint64_t unix_end_time = span_.start_time_unix_nano() + duration.count();
  span_.set_end_time_unix_nano(unix_end_time);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
