#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/sdk/trace/multi_span_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

TracerContext::TracerContext(std::vector<std::unique_ptr<SpanProcessor>> &&processors,
                             opentelemetry::sdk::resource::Resource resource,
                             std::unique_ptr<Sampler> sampler,
                             std::unique_ptr<IdGenerator> id_generator) noexcept
    : processor_(std::unique_ptr<SpanProcessor>(new MultiSpanProcessor(std::move(processors)))),
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

void TracerContext::AddProcessor(std::unique_ptr<SpanProcessor> processor) noexcept
{

  auto multi_processor = static_cast<MultiSpanProcessor *>(processor_.get());
  multi_processor->AddProcessor(std::move(processor));
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
