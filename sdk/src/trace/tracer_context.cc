#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

TracerContext::TracerContext(std::unique_ptr<SpanProcessor> processor,
                             opentelemetry::sdk::resource::Resource resource,
                             std::unique_ptr<Sampler> sampler,
                             std::unique_ptr<IdGenerator> id_generator) noexcept
    : processor_(std::move(processor)),
      resource_(resource),
      sampler_(std::move(sampler)),
      id_generator_(std::move(id_generator))
{}

Sampler &TracerContext::GetSampler() const noexcept
{
  return *sampler_;
}

const opentelemetry::sdk::resource::Resource &TracerContext::GetResource() const noexcept
{
  return resource_;
}

opentelemetry::sdk::trace::IdGenerator &TracerContext::GetIdGenerator() const noexcept
{
  return *id_generator_;
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

bool TracerContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return processor_->ForceFlush(timeout);
}

bool TracerContext::Shutdown() noexcept
{
  return processor_->Shutdown();
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
