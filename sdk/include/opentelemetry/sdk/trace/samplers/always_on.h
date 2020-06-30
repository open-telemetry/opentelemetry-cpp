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
   * @return Always return Decision RECORD_AND_SAMPLE
   */
  inline SamplingResult ShouldSample(const SpanContext * /*parent_context*/,
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
  inline std::string GetDescription() const noexcept override { return "AlwaysOnSampler"; }
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
