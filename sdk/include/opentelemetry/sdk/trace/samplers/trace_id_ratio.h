#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;
/**
 * The TraceIdRatioBased sampler computes and returns a decision based on the
 * provided trace_id and the configured ratio.
 */
class TraceIdRatioBasedSampler : public Sampler
{
public:
  /**
   * @param ratio a required value, 1.0 >= ratio >= 0.0. If the given trace_id
   * falls into a given ratio of all possible trace_id values, ShouldSample will
   * return RECORD_AND_SAMPLE.
   * @throws invalid_argument if ratio is out of bounds [0.0, 1.0]
   */
  explicit TraceIdRatioBasedSampler(double ratio);

  /**
   * @return Returns either RECORD_AND_SAMPLE or DROP based on current
   * sampler configuration and provided trace_id and ratio. trace_id
   * is used as a pseudorandom value in conjunction with the predefined
   * ratio to determine whether this trace should be sampled
   */
  SamplingResult ShouldSample(const trace_api::SpanContext & /*parent_context*/,
                              trace_api::TraceId trace_id,
                              nostd::string_view /*name*/,
                              trace_api::SpanKind /*span_kind*/,
                              const trace_api::KeyValueIterable & /*attributes*/) noexcept override;

  /**
   * @return Description MUST be TraceIdRatioBasedSampler{0.000100}
   */
  nostd::string_view GetDescription() const noexcept override;

private:
  std::string description_;
  const uint64_t threshold_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
