#include "exporters/otlp/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
// void Recordable::SetIds(trace::TraceId trace_id,
//                         trace::SpanId span_id,
//                         trace::SpanId parent_span_id) noexcept {
//   span_.set_trace_id(trace_id);
// }

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
  uint64_t nano_unix_time = std::chrono::duration_cast<std::chrono::milliseconds>(nano_time).count();
  span_.set_start_time_unixnano(nano_unix_time);
}

// void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept {

// }
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
