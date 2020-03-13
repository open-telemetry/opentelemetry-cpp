#include "opentelemetry/sdk/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
Tracer &Span::tracer() const noexcept
{
  return *tracer_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
