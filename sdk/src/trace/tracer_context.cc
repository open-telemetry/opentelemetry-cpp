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

Sampler &TracerContext::GetSampler() const noexcept
{
  return *sampler_.get();
}

const opentelemetry::sdk::resource::Resource &TracerContext::GetResource() const noexcept
{
  return resource_;
}

void TracerContext::RegisterPipeline(std::unique_ptr<SpanProcessor> processor) noexcept
{
  // TODO(jsuereth): Implement
  // 1. If existing processor is an "AggregateProcessor" append the new processor to it.
  // 2. If the existing processor is NOT an "AggregateProcessor", create a new Aggregate of this and
  // the other,
  //    then replace our atomic ptr with the new aggregate.
}

SpanProcessor &TracerContext::GetActiveProcessor() const noexcept
{
  return *processor_;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
