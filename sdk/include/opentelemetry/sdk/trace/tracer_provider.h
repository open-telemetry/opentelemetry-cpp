#pragma once

#include <map>
#include <memory>
#include <string>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/tracer.h"
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
   * Initialize a new tracer provider with a default AlwaysOn sampler.
   * @param processor The span processor for this tracer provider. This must
   * not be a nullptr.
   */
  explicit TracerProvider(std::shared_ptr<SpanProcessor> processor) noexcept;

  /**
   * Initialize a new tracer provider with a specified sampler
   * @param processor The span processor for this tracer provider. This must
   * not be a nullptr.
   * @param sampler The sampler for this tracer provider. This must
   * not be a nullptr.
   */
  explicit TracerProvider(std::shared_ptr<SpanProcessor> processor,
                          std::shared_ptr<Sampler> sampler) noexcept;

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> GetTracer(
      nostd::string_view library_name,
      nostd::string_view library_version = "") noexcept override;

  /**
   * Set the span processor associated with this tracer provider.
   * @param processor The new span processor for this tracer provider. This
   * must not be a nullptr.
   */
  void SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept;

  /**
   * Set the sampler associated with this tracer provider.
   * @param sampler The new sampler for this tracer provider. This
   * must not be a nullptr.
   */
  void SetSampler(std::shared_ptr<Sampler> sampler) noexcept;

  /**
   * Obtain the span processor associated with this tracer provider.
   * @return The span processor for this tracer provider.
   */
  std::shared_ptr<SpanProcessor> GetProcessor() const noexcept;

  /**
   * Obtain the sampler associated with this tracer provider.
   * @return The span processor for this tracer provider.
   */
  std::shared_ptr<Sampler> GetSampler() const noexcept;

private:
  opentelemetry::sdk::AtomicSharedPtr<SpanProcessor> processor_;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer_;
  opentelemetry::sdk::AtomicSharedPtr<Sampler> sampler_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
