#include "opentelemetry/sdk/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
TracerProvider::TracerProvider(std::shared_ptr<SpanProcessor> processor,
                               opentelemetry::sdk::resource::Resource &&resource,
                               std::shared_ptr<Sampler> sampler) noexcept
    : processor_{processor},
      tracer_(new Tracer(std::move(processor), resource, sampler)),
      sampler_(sampler),
      resource_(resource)
{}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> TracerProvider::GetTracer(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  return opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer>(tracer_);
}

void TracerProvider::SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept
{
  processor_.store(processor);

  auto sdkTracer = static_cast<Tracer *>(tracer_.get());
  sdkTracer->SetProcessor(processor);
}

std::shared_ptr<SpanProcessor> TracerProvider::GetProcessor() const noexcept
{
  return processor_.load();
}

std::shared_ptr<Sampler> TracerProvider::GetSampler() const noexcept
{
  return sampler_;
}

const opentelemetry::sdk::resource::Resource &TracerProvider::GetResource() const noexcept
{
  return resource_;
}

bool TracerProvider::Shutdown() noexcept
{
  return GetProcessor()->Shutdown();
}

bool TracerProvider::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return GetProcessor()->Shutdown(timeout);
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
