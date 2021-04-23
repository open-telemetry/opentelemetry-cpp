#include "opentelemetry/sdk/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
TracerProvider::TracerProvider(std::shared_ptr<sdk::trace::TracerContext> context) noexcept
    : context_{context}
{}

TracerProvider::TracerProvider(std::unique_ptr<SpanProcessor> processor,
                               opentelemetry::sdk::resource::Resource resource,
                               std::unique_ptr<Sampler> sampler,
                               std::unique_ptr<IdGenerator> id_generator) noexcept
    : TracerProvider(std::make_shared<TracerContext>(std::move(processor),
                                                     resource,
                                                     std::move(sampler),
                                                     std::move(id_generator)))
{}

opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> TracerProvider::GetTracer(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  // if (library_name == "") {
  //   // TODO: log invalid name.
  // }

  auto lib = InstrumentationLibrary::create(library_name, library_version);
  for (auto &tracer : tracers_)
  {
    auto &tracer_lib =
        dynamic_cast<sdk::trace::Tracer *>(tracer.get())->GetInstrumentationLibrary();
    if (tracer_lib == *lib)
    {
      return tracer;
    }
  }
  auto tracer = new sdk::trace::Tracer(context_, std::move(lib));
  tracers_.push_back(nostd::shared_ptr<opentelemetry::trace::Tracer>(std::move(tracer)));

  return tracers_.back();
}

void TracerProvider::RegisterPipeline(std::unique_ptr<SpanProcessor> processor) noexcept
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
