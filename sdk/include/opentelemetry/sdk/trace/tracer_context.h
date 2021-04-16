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
 * - The owner/destroyer of Processors/Exporters.  These will remain active until
 *   this class is destroyed.  I.e. Sampling, Exporting, flushing etc. are all ok if this
 *   object is alive, and they will work together. If this object is destroyed, then
 *   no shared references to Processor, Exporter, Recordable etc. should exist, and all
 *   associated pipelines will have been flushed.
 */
class TracerContext
{
public:
  explicit TracerContext(std::unique_ptr<SpanProcessor> processor,
                         opentelemetry::sdk::resource::Resource resource =
                             opentelemetry::sdk::resource::Resource::Create({}),
                         std::unique_ptr<Sampler> sampler =
                             std::unique_ptr<AlwaysOnSampler>(new AlwaysOnSampler)) noexcept;
  /**
   * Attaches a span processor to this tracer context.
   *
   * @param processor The new span processor for this tracer. This must not be
   * a nullptr. Ownership is given to the `TracerContext`.
   */
  void RegisterPipeline(std::unique_ptr<SpanProcessor> processor) noexcept;

  /**
   * Obtain the sampler associated with this tracer.
   * @return The sampler for this tracer.
   */
  Sampler &GetSampler() const noexcept;

  /**
   * Obtain the (conceptual) active processor.
   *
   * Note: When more than one processor is active, this will
   * return an "aggregate" processor
   */
  SpanProcessor &GetActiveProcessor() const noexcept;

  /**
   * Obtain the resource associated with this tracer context.
   * @return The resource for this tracer context.
   */
  const opentelemetry::sdk::resource::Resource &GetResource() const noexcept;

  /**
   * Force all active SpanProcessors to flush any buffered spans
   * within the given timeout.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

  /**
   * Shutdown the span processor associated with this tracer provider.
   */
  bool Shutdown() noexcept;

private:
  // This is an atomic pointer so we can adapt the processor pipeline dynamically.
  opentelemetry::sdk::common::AtomicUniquePtr<SpanProcessor> processor_;
  opentelemetry::sdk::resource::Resource resource_;
  std::unique_ptr<Sampler> sampler_;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
