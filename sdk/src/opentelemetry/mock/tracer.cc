#include "sdk/src/opentelemetry/mock/tracer.h"

#include "sdk/src/opentelemetry/mock/span.h"

namespace opentelemetry
{
namespace sdk
{
namespace mock
{
Tracer::Tracer(TracerConfig &&config) noexcept
{
  (void)config;
}

nostd::unique_ptr<trace::Span> Tracer::StartSpan(nostd::string_view name,
                                                 const trace::StartSpanOptions &options) noexcept
{
  return nostd::unique_ptr<trace::Span>{new (std::nothrow) Span{this->shared_from_this()}};
}
}  // namespace mock
}  // namespace sdk
}  // namespace opentelemetry
