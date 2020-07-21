#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;
/**
 * The probability sampler, based on it's configuration, should either defer the
 * decision to sample to it's parent, or compute and return a decision based on
 * the provided trace_id and probability.
 */
class ProbabilitySampler : public Sampler
{
public:
  /**
   * @param probability a required value, 1.0 >= probability >= 0.0, that given any
   * random trace_id, ShouldSample will return RECORD_AND_SAMPLE
   * @throws invalid_argument if probability is out of bounds [0.0, 1.0]
   */
  explicit ProbabilitySampler(double probability);

  /**
   * @return Returns either RECORD_AND_SAMPLE or NOT_RECORD based on current
   * sampler configuration and provided parent_context / tracer_id. tracer_id
   * is used as a pseudorandom value in conjunction with the predefined
   * threshold to determine whether this trace should be sampled
   */
  SamplingResult ShouldSample(
    const trace_api::SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view /*name*/,
    trace_api::SpanKind /*span_kind*/,
    const trace_api::KeyValueIterable & /*attributes*/) noexcept override;

  /**
   * @return Description MUST be ProbabilitySampler{0.000100}
   */
  nostd::string_view GetDescription() const noexcept override;

private:
  std::string description_;
  const uint64_t threshold_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
