#include "src/exporter/otprotocol/recordable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace exporter
{
namespace otprotocol
{
void Recordable::AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
{
  (void)name;
}

void Recordable::SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept
{
  (void)code;
  (void)description;
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_.set_name(name.data(), name.size());
}
}  // namespace otprotocol
}  // namespace exporter
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
