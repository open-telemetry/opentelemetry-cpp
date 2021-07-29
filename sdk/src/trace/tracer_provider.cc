// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/sdk_config.h"

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
{
  std::vector<std::unique_ptr<SpanProcessor>> processors;
  processors.push_back(std::move(processor));
  context_ = std::make_shared<TracerContext>(std::move(processors), resource, std::move(sampler),
                                             std::move(id_generator));
}

TracerProvider::TracerProvider(std::vector<std::unique_ptr<SpanProcessor>> &&processors,
                               opentelemetry::sdk::resource::Resource resource,
                               std::unique_ptr<Sampler> sampler,
                               std::unique_ptr<IdGenerator> id_generator) noexcept
{
  context_ = std::make_shared<TracerContext>(std::move(processors), resource, std::move(sampler),
                                             std::move(id_generator));
}

nostd::shared_ptr<opentelemetry::trace::Tracer> TracerProvider::GetTracer(
    nostd::string_view library_name,
    nostd::string_view library_version) noexcept
{
  if (library_name.data() == nullptr)
  {
    library_name = "";
  }
  if (library_name == "")
  {
    OTEL_INTERNAL_LOG_ERROR("[TracerProvider::GetTracer] Library name is empty.");
  }

  const std::lock_guard<std::mutex> guard(lock_);

  for (auto &tracer : tracers_)
  {
    auto &tracer_lib = tracer->GetInstrumentationLibrary();
    if (tracer_lib.equal(library_name, library_version))
    {
      return nostd::shared_ptr<opentelemetry::trace::Tracer>{tracer};
    }
  }

  auto lib = InstrumentationLibrary::Create(library_name, library_version);
  tracers_.push_back(std::shared_ptr<opentelemetry::sdk::trace::Tracer>(
      new sdk::trace::Tracer(context_, std::move(lib))));
  return nostd::shared_ptr<opentelemetry::trace::Tracer>{tracers_.back()};
}

void TracerProvider::AddProcessor(std::unique_ptr<SpanProcessor> processor) noexcept
{
  context_->AddProcessor(std::move(processor));
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
