#include "sdk/src/trace/tracer.h"

namespace opentelemetry
{
namespace sdk
{
namespace trace
{
nostd::unique_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const trace_api::StartSpanOptions &options) noexcept
{
  (void)name;
  (void)options;
  return nullptr;
}
}  // namespace trace
}  // namespace sdk
}  // namespace opentelemetry
