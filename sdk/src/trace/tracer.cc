#include "opentelemetry/sdk/trace/tracer.h"

#include "opentelemetry/version.h"
#include "src/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
void Tracer::SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept
{
  processor_ = processor;
}

SpanProcessor &Tracer::GetProcessor() const noexcept
{
  return *processor_;
}

nostd::unique_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const trace_api::StartSpanOptions &options) noexcept
{
  return nostd::unique_ptr<trace_api::Span>{new (std::nothrow)
                                                Span{this->shared_from_this(), name, options}};
}

void Tracer::ForceFlushWithMicroseconds(uint64_t timeout) noexcept
{
  (void)timeout;
}

void Tracer::CloseWithMicroseconds(uint64_t timeout) noexcept
{
  (void)timeout;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
