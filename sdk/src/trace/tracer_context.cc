#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/multi_span_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

TracerContext::TracerContext(std::vector<std::unique_ptr<SpanProcessor>> &&processors,
                             opentelemetry::sdk::resource::Resource resource,
                             std::unique_ptr<Sampler> sampler) noexcept
    : resource_(resource),
      sampler_(std::move(sampler)),
      processor_(std::unique_ptr<SpanProcessor>(new MultiSpanProcessor(std::move(processors))))
{}

Sampler &TracerContext::GetSampler() const noexcept
{
  return *sampler_.get();
}

const opentelemetry::sdk::resource::Resource &TracerContext::GetResource() const noexcept
{
  return resource_;
}

void TracerContext::AddProcessor(std::unique_ptr<SpanProcessor> processor) noexcept
{

  auto multi_processor = static_cast<MultiSpanProcessor *>(processor_.Get());
  multi_processor->AddProcessor(std::move(processor));
  // TODO(jsuereth): Implement
  // 1. If existing processor is an "AggregateProcessor" append the new processor to it.
  // 2. If the existing processor is NOT an "AggregateProcessor", create a new Aggregate of this and
  // the other,
  //    then replace our atomic ptr with the new aggregate.
}

SpanProcessor &TracerContext::GetProcessor() const noexcept
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
