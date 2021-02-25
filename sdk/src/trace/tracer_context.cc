#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

TracerContext::TracerContext(std::unique_ptr<SpanProcessor> processor,
                             opentelemetry::sdk::resource::Resource resource,
                             std::unique_ptr<Sampler> sampler) noexcept
    : processor_(std::move(processor)), resource_(resource), sampler_(std::move(sampler))
{}

SpanProcessor *TracerContext::GetProcessor() const noexcept
{
  return processor_.Get();
}

Sampler *TracerContext::GetSampler() const noexcept
{
  return sampler_.Get();
}

const opentelemetry::sdk::resource::Resource &TracerContext::GetResource() const noexcept
{
  return resource_;
}

void TracerContext::SetProcessor(std::unique_ptr<SpanProcessor> processor) noexcept
{
  processor_.Reset(processor.release());
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
