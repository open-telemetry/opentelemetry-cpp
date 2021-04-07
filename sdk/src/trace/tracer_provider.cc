#include "opentelemetry/sdk/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
TracerProvider::TracerProvider(std::shared_ptr<sdk::trace::TracerContext> context) noexcept
    : context_{context}, tracer_(new Tracer(context))
{}

TracerProvider::TracerProvider(std::unique_ptr<SpanProcessor> processor,
                               opentelemetry::sdk::resource::Resource resource,
                               std::unique_ptr<Sampler> sampler) noexcept
    : TracerProvider(
          std::make_shared<TracerContext>(std::move(processor), resource, std::move(sampler)))
{}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> TracerProvider::GetTracer(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer>(tracer_);
}

void TracerProvider::RegisterProcessor(std::unique_ptr<SpanProcessor> processor) noexcept
{
  return context_->RegisterPipeline(std::move(processor));
}

const opentelemetry::sdk::resource::Resource &TracerProvider::GetResource() const noexcept
{
  return context_->GetResource();
}

bool TracerProvider::Shutdown() noexcept
{
  return context_->Shutdown();
}

bool TracerProvider::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return context_->ForceFlush(timeout);
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
