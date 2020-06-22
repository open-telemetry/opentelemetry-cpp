#include "exporters/otlp/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
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
  span_.set_start_time_unixnano(start_time.time_since_epoch());
}
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
