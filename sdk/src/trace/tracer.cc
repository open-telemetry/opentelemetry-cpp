#include "src/trace/tracer.h"

#include "opentelemetry/version.h"
#include "src/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
nostd::unique_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const trace_api::StartSpanOptions &options) noexcept
{
  return nostd::unique_ptr<trace_api::Span>{new (std::nothrow)
                                                Span{this->shared_from_this(), name, options}};
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
