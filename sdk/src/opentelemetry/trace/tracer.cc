#include "opentelemetry/sdk/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
nostd::unique_ptr<opentelemetry::trace::Span> Tracer::StartSpan(
    nostd::string_view name,
    const opentelemetry::trace::StartSpanOptions &options) noexcept
{
  // TODO: make a span!
  return nullptr;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
