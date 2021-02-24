#pragma once

#include "opentelemetry/sdk/common/atomic_unique_ptr.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * A class which stores the TracerProvider context.
 *
 * This class meets the following design criteria:
 * - A shared reference between TracerProvider and Tracers instantiated.
 * - A thread-safe class that allows updating/altering processor/exporter pipelines
 *   and sampling config.
 * - The owner/destoryer of Processors/Exporters.  These will remain active until
 *   this class is destroyed.
 */
class TracerContext
{
public:
  explicit TracerContext(std::unique_ptr<SpanProcessor> processor,
                         opentelemetry::sdk::resource::Resource &&resource =
                             opentelemetry::sdk::resource::Resource::Create({}),
                         std::unique_ptr<Sampler> sampler =
                             std::unique_ptr<AlwaysOnSampler>(new AlwaysOnSampler)) noexcept;
  /**
   * Obtain the span processor assocaited with this tracer context.
   * <p>
   * This does NOT give ownership to the caller and must be used within the lifecycle of
   * a shared `TracerContext` object.
   */
  SpanProcessor *GetProcessor() const noexcept;
  /**
   * Set the span processor associated with this tracer.
   * @param processor The new span processor for this tracer. This must not be
   * a nullptr.
   */
  void SetProcessor(std::unique_ptr<SpanProcessor> &&processor) noexcept;

  /**
   * Obtain the sampler associated with this tracer.
   * @return The sampler for this tracer.
   */
  Sampler *GetSampler() const noexcept;

  /**
   * Obtain the resource associated with this tracer context.
   * @return The resource for this tracer context.
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept;

private:
  opentelemetry::sdk::common::AtomicUniquePtr<SpanProcessor> processor_;
  opentelemetry::sdk::common::AtomicUniquePtr<Sampler> sampler_;
  opentelemetry::sdk::resource::Resource resource_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE