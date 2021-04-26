#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

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
  explicit TracerProvider(
      std::unique_ptr<SpanProcessor> processor,
      opentelemetry::sdk::resource::Resource resource =
          opentelemetry::sdk::resource::Resource::Create({}),
      std::unique_ptr<Sampler> sampler = std::unique_ptr<AlwaysOnSampler>(new AlwaysOnSampler),
      std::unique_ptr<opentelemetry::sdk::trace::IdGenerator> id_generator =
          std::unique_ptr<opentelemetry::sdk::trace::IdGenerator>(
              new RandomIdGenerator())) noexcept;

  /**
   * Initialize a new tracer provider with a specified context
   * @param context The shared tracer configuration/pipeline for this provider.
   */
  explicit TracerProvider(std::shared_ptr<sdk::trace::TracerContext> context) noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

  /**
   * Attaches a span processor pipeline to this tracer provider.
   * @param processor The new span processor for this tracer provider. This
   * must not be a nullptr.
   *
   * Note: This process may not receive any in-flight spans, but will get newly created spans.
   */
  void RegisterPipeline(std::unique_ptr<SpanProcessor> processor) noexcept;

  /**
   * Obtain the resource associated with this tracer provider.
   * @return The resource for this tracer provider.
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Shutdown the span processor associated with this tracer provider.
   */
  bool Shutdown() noexcept;

  /**
   * Force flush the span processor associated with this tracer provider.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

private:
  std::shared_ptr<sdk::trace::TracerContext> context_;
  std::vector<std::shared_ptr<opentelemetry::sdk::trace::Tracer>> tracers_;
  std::mutex lock_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
