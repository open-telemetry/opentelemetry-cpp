#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{


TracerContext::TracerContext(std::shared_ptr<SpanProcessor> processor,
                             opentelemetry::sdk::resource::Resource resource,
                             std::unique_ptr<Sampler> sampler) noexcept
    : processor_(processor), resource_(resource), sampler_(std::move(sampler))
{}

SpanProcessor *TracerContext::GetProcessor() const noexcept
{
  return processor_.load().get();
}

Sampler *TracerContext::GetSampler() const noexcept
{
  return sampler_.Get();
}

const opentelemetry::sdk::resource::Resource &TracerContext::GetResource() const noexcept
{
  return resource_;
}

void TracerContext::SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept
{
  processor_.store(processor);
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
