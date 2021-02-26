#pragma once

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
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
 *   this class is destroyed.  I.e. Sampling, Exporting, flushing etc. are all ok if this
 *   object is alive, and they willl work together.   If this object is destroyed, then
 *   no shared references to Processor, Exporter, Recordable etc. should exist, and all
 *   associated pipelines will have been flushed.
 *
 *
 * TODOs - This should allow more than one processor pipeline to be attached.
 */
class TracerContext
{
public:
  explicit TracerContext(std::shared_ptr<SpanProcessor> processor,
                         opentelemetry::sdk::resource::Resource resource =
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
  void SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept;

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
  // Note:  Currently Z-Pages Exporter relies on sharing processor w/ HTTP server.
  // Likely this should be decoupled going forward, for cleaner shutdown semantics
  // and ownership on pipelines.
  opentelemetry::sdk::AtomicSharedPtr<SpanProcessor> processor_;
  opentelemetry::sdk::resource::Resource resource_;
  opentelemetry::sdk::common::AtomicUniquePtr<Sampler> sampler_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE