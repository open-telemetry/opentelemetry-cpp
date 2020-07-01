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
 * A placeholder class that stores the states of a span.
 * Will be replaced by the real SpanContext class once it is implemented.
 */
class Sampler::SpanContext
{
public:
  inline explicit SpanContext(bool is_recording, bool sampled_flag)
      : is_recording(is_recording), sampled_flag(sampled_flag)
  {}

  bool is_recording;
  bool sampled_flag;
};
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
  SamplingResult ShouldSample(const SpanContext * /*parent_context*/,
                              trace_api::TraceId /*trace_id*/,
                              nostd::string_view /*name*/,
                              trace_api::SpanKind /*span_kind*/,
                              const trace_api::KeyValueIterable & /*attributes*/) noexcept override;

  /**
   * @return Description MUST be ParentOrElse{delegate_sampler_.getDescription()}
   */
  std::string GetDescription() const noexcept override;

private:
  std::shared_ptr<Sampler> delegate_sampler_;
  std::string description_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
