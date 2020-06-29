#include "opentelemetry/sdk/trace/tracer.h"

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/version.h"
#include "src/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
Tracer::Tracer(std::shared_ptr<SpanProcessor> processor) noexcept
    : Tracer::Tracer(processor, std::make_shared<AlwaysOnSampler>())
{}

Tracer::Tracer(std::shared_ptr<SpanProcessor> processor, std::shared_ptr<Sampler> sampler) noexcept
    : processor_{processor}, sampler_{sampler}
{}

void Tracer::SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept
{
  processor_.store(processor);
}

std::shared_ptr<SpanProcessor> Tracer::GetProcessor() const noexcept
{
  return processor_.load();
}

void Tracer::SetSampler(std::shared_ptr<Sampler> sampler) noexcept
{
  sampler_.store(sampler);
}

std::shared_ptr<Sampler> Tracer::GetSampler() const noexcept
{
  return sampler_.load();
}

nostd::unique_ptr<trace_api::Span> Tracer::StartSpan(
    nostd::string_view name,
    const trace_api::KeyValueIterable &attributes,
    const trace_api::StartSpanOptions &options) noexcept
{
  return nostd::unique_ptr<trace_api::Span>{new (std::nothrow) Span{
      this->shared_from_this(), processor_.load(), name, attributes, options}};
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
