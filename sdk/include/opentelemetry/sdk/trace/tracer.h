#pragma once

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/trace/tracer_context.h"
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
  explicit Tracer(std::shared_ptr<sdk::trace::TracerContext> context) noexcept;

  nostd::shared_ptr<trace_api::Span> StartSpan(
      nostd::string_view name,
      const opentelemetry::common::KeyValueIterable &attributes,
      const trace_api::SpanContextKeyValueIterable &links,
      const trace_api::StartSpanOptions &options = {}) noexcept override;

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override;

  void CloseWithMicroseconds(uint64_t timeout) noexcept override;

  SpanProcessor& GetActiveProcessor() noexcept {
    return context_->GetActiveProcessor();
  }


  // Note: Test only
  Sampler& GetSampler() { return context_->GetSampler(); }

private:
  std::shared_ptr<sdk::trace::TracerContext> context_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
