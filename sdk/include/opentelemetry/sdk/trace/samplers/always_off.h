#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

/**
 * The always off sampler always returns IGNORE, effectively disabling
 * tracing functionality.
 */
class AlwaysOffSampler : public Sampler
{
public:
  /**
   * @return Returns IGNORE always
   */
  SamplingResult ShouldSample(const trace_api::SpanContext * /*parent_context*/,
                              trace_api::TraceId /*trace_id*/,
                              nostd::string_view /*name*/,
                              trace_api::SpanKind /*span_kind*/,
                              const trace_api::KeyValueIterable & /*attributes*/) noexcept override
  {
    return {Decision::IGNORE, nullptr};
  }

  /**
   * @return Description MUST be AlwaysOffSampler
   */
  nostd::string_view GetDescription() const noexcept override { return "AlwaysOffSampler"; }
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
