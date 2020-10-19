#pragma once

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Tracer final : public trace_api::Tracer, public std::enable_shared_from_this<Tracer>
{
public:
  /**
   * Initialize a new tracer.
   * @param processor The span processor for this tracer. This must not be a
   * nullptr.
   */
  explicit Tracer(std::shared_ptr<SpanProcessor> processor,
                  std::shared_ptr<Sampler> sampler = std::make_shared<AlwaysOnSampler>()) noexcept;

  /**
   * Set the span processor associated with this tracer.
   * @param processor The new span processor for this tracer. This must not be
   * a nullptr.
   */
  void SetProcessor(std::shared_ptr<SpanProcessor> processor) noexcept;

  /**
   * Obtain the span processor associated with this tracer.
   * @return The span processor for this tracer.
   */
  std::shared_ptr<SpanProcessor> GetProcessor() const noexcept;

  /**
   * Obtain the sampler associated with this tracer.
   * @return The sampler for this tracer.
   */
  std::shared_ptr<Sampler> GetSampler() const noexcept;

  nostd::shared_ptr<trace_api::Span> StartSpan(
      nostd::string_view name,
      const opentelemetry::common::KeyValueIterable &attributes,
      const trace_api::StartSpanOptions &options = {}) noexcept override;

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override;

  void CloseWithMicroseconds(uint64_t timeout) noexcept override;

private:
  opentelemetry::sdk::AtomicSharedPtr<SpanProcessor> processor_;
  const std::shared_ptr<Sampler> sampler_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
