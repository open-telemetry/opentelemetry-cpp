#include "opentelemetry/sdk/trace/tracer.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
Tracer::Tracer(nostd::string_view name, nostd::string_view version) : name_{name}, version_{version}
{}
nostd::unique_ptr<opentelemetry::trace::Span> Tracer::StartSpan(
    nostd::string_view name,
    const opentelemetry::trace::StartSpanOptions &options) noexcept
{
  // TODO: make a span!
  return nullptr;
}
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
