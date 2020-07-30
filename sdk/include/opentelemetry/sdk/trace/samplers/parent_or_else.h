#pragma once

#include "opentelemetry/sdk/common/atomic_shared_ptr.h"
#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

/**
 * The parent or else sampler is a composite sampler. ParentOrElse(delegateSampler) either respects
 * the parent span's sampling decision or delegates to delegateSampler for root spans.
 */
class ParentOrElseSampler : public Sampler
{
public:
  explicit ParentOrElseSampler(std::shared_ptr<Sampler> delegate_sampler) noexcept;
  /** The decision either respects the parent span's sampling decision or delegates to
   * delegateSampler for root spans
   * @return Returns NOT_RECORD always
   */
  SamplingResult ShouldSample(const trace_api::SpanContext *parent_context,
                              trace_api::TraceId trace_id,
                              nostd::string_view name,
                              trace_api::SpanKind span_kind,
                              const trace_api::KeyValueIterable &attributes) noexcept override;

  /**
   * @return Description MUST be ParentOrElse{delegate_sampler_.getDescription()}
   */
  nostd::string_view GetDescription() const noexcept override;

private:
  const std::shared_ptr<Sampler> delegate_sampler_;
  const std::string description_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
