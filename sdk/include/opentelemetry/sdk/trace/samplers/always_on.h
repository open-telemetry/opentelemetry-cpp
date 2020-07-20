#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

/**
 * The always on sampler is a default sampler which always return Decision::RECORD_AND_SAMPLE
 */
class AlwaysOnSampler : public Sampler
{
public:
  /**
   * The constructor sets the description to "AlwaysOnSampler".
   */
  AlwaysOnSampler(): description_("AlwaysOnSampler") {}
  /**
   * @return Always return Decision RECORD_AND_SAMPLE
   */
  inline SamplingResult ShouldSample(const trace_api::SpanContext * /*parent_context*/,
                              trace_api::TraceId /*trace_id*/,
                              nostd::string_view /*name*/,
                              trace_api::SpanKind /*span_kind*/,
                              const trace_api::KeyValueIterable & /*attributes*/) noexcept override
  {
    return {Decision::RECORD_AND_SAMPLE, nullptr};
  }

  /**
   * @return Description MUST be AlwaysOnSampler
   */
  inline nostd::string_view GetDescription() const noexcept override { return description_; }

private:
  std::string description_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
