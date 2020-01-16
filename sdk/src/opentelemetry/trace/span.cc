#include "opentelemetry/sdk/trace/span.h"

namespace opentelemetry
{
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
}  // namespace opentelemetry
