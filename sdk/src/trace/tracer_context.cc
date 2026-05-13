// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <chrono>
#include <memory>
#include <utility>
#include <vector>

#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/id_generator.h"
#include "opentelemetry/sdk/trace/multi_span_processor.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/tracer_config.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace resource = opentelemetry::sdk::resource;

TracerContext::TracerContext(std::vector<std::unique_ptr<SpanProcessor>> &&processors,
                             const resource::Resource &resource,
                             std::unique_ptr<Sampler> sampler,
                             std::unique_ptr<IdGenerator> id_generator,
                             std::unique_ptr<instrumentationscope::ScopeConfigurator<TracerConfig>>
                                 tracer_configurator) noexcept
    : resource_(resource),
      sampler_(std::move(sampler)),
      id_generator_(std::move(id_generator)),
      tracer_configurator_(std::move(tracer_configurator)),
      multi_processor_(nullptr)
{
  if (processors.empty())
  {
    processor_ = std::unique_ptr<SpanProcessor>(new MultiSpanProcessor(std::move(processors)));
  }
  else
  {
    // at least one processor is available here
    for (auto &&processor : processors)
    {
      AddProcessor(std::move(processor));
    }
  }
}

Sampler &TracerContext::GetSampler() const noexcept
{
  return *sampler_;
}

const resource::Resource &TracerContext::GetResource() const noexcept
{
  return resource_;
}

const instrumentationscope::ScopeConfigurator<TracerConfig> &TracerContext::GetTracerConfigurator()
    const noexcept
{
  return *tracer_configurator_;
}

opentelemetry::sdk::trace::IdGenerator &TracerContext::GetIdGenerator() const noexcept
{
  return *id_generator_;
}

void TracerContext::AddProcessor(std::unique_ptr<SpanProcessor> processor) noexcept
{
  if (!processor)
  {
    return;
  }

  if (!processor_)
  {
    // this is the first processor to be added
    processor_ = std::move(processor);
  }
  else if (multi_processor_ == nullptr)
  {
    // a processor exists, but it's not a MultiSpanProcessor. make a new MultiSpanProcessor
    std::unique_ptr<MultiSpanProcessor> multi_processor(new MultiSpanProcessor({}));
    multi_processor->AddProcessor(std::move(processor_));
    multi_processor->AddProcessor(std::move(processor));

    // duplicate the pointer before it gets type erased
    multi_processor_ = multi_processor.get();

    processor_ = std::move(multi_processor);
  }
  else /*if (multi_processor_ != nullptr)*/
  {
    // already have a MultiSpanProcessor, add the processor to it
    multi_processor_->AddProcessor(std::move(processor));
  }
}

SpanProcessor &TracerContext::GetProcessor() const noexcept
{
  return *processor_;
}

bool TracerContext::ForceFlush(std::chrono::microseconds timeout) noexcept
{
  return processor_->ForceFlush(timeout);
}

bool TracerContext::Shutdown(std::chrono::microseconds timeout) noexcept
{
  return processor_->Shutdown(timeout);
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
