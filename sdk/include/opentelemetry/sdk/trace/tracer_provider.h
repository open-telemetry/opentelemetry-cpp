#pragma once

#include <map>
#include <memory>
#include <string>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
#include "opentelemetry/trace/tracer_provider.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class TracerProvider final : public opentelemetry::trace::TracerProvider
{
public:
  /**
   * Initialize a new tracer provider with a specified sampler
   * @param processor The span processor for this tracer provider. This must
   * not be a nullptr.
   * @param sampler The sampler for this tracer provider. This must
   * not be a nullptr.
   */
  explicit TracerProvider(std::shared_ptr<SpanProcessor> processor,
                         opentelemetry::sdk::resource::Resource resource =
                             opentelemetry::sdk::resource::Resource::Create({}),
                         std::unique_ptr<Sampler> sampler =
                             std::unique_ptr<AlwaysOnSampler>(new AlwaysOnSampler)) noexcept;

  /**
   * Initialize a new tracer provider with a specified context
   * @param context The shared tracer configuraiton/pipeline for this provider.
   */
  explicit TracerProvider(
      std::shared_ptr<sdk::trace::TracerContext> context) noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

  /**
   * Set the span processor associated with this tracer provider.
   * @param processor The new span processor for this tracer provider. This
   * must not be a nullptr.
   */
  void SetProcessor(std::unique_ptr<SpanProcessor> processor) noexcept;

  /**
   * Obtain the span processor associated with this tracer provider.
   * @return The span processor for this tracer provider.
   */
  SpanProcessor* GetProcessor() const noexcept;

  /**
   * Obtain the sampler associated with this tracer provider.
   * @return The sampler for this tracer provider.
   */
  Sampler* GetSampler() const noexcept;

  /**
   * Obtain the resource associated with this tracer provider.
   * @return The resource for this tracer provider.
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Shutdown the span processor associated with this tracer provider.
   */
  bool Shutdown() noexcept;

private:
  std::shared_ptr<sdk::trace::TracerContext> context_;
  // TODO: We should have one-tracer per-instrumentation library, per specification.
  std::shared_ptr<opentelemetry::trace::Tracer> tracer_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
