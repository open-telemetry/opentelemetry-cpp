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
  const uint8_t* trace_id_data = trace_id.Id().data();
  const char* trace_id_buff = (char*)trace_id_data;

  const uint8_t* span_id_data = span_id.Id().data();
  const char* span_id_buff = (char*)span_id_data;

  const uint8_t* parent_span_id_data = parent_span_id.Id().data();
  const char* parent_span_id_buff = (char*)parent_span_id_data;

  span_.set_trace_id(trace_id_buff);
  span_.set_span_id(span_id_buff);
  span_.set_parent_span_id(parent_span_id_buff);
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
  std::chrono::nanoseconds nano_time = start_time.time_since_epoch();
  uint64_t nano_unix_time = nano_time.count();
  span_.set_start_time_unixnano(nano_unix_time);
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  uint64_t unix_duration = duration.count();
  uint64_t unix_end_time = span_.start_time_unixnano() + unix_duration;
  span_.set_end_time_unixnano(unix_end_time);
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
