#pragma once

#include "opentelemetry/sdk/trace/sampler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;
/**
 * The probability sampler ...
 *
 */
class ProbabilitySampler : public Sampler
{
public:
  explicit ProbabilitySampler(double probability);

  /**
   * @return Returns NOT_RECORD always
   */
  SamplingResult ShouldSample(
    const SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view name,
    trace_api::SpanKind span_kind,
    const trace_api::KeyValueIterable &attributes) noexcept override;

  /**
   * @return Description MUST be ProbabilitySampler{0.000100}
   */
  std::string GetDescription() const noexcept override;

  uint64_t CalculateThreshold(double probability) const noexcept;

  uint64_t CalculateThresholdFromBuffer(const trace_api::TraceId& trace_id) const noexcept;

  double GetProbability() const noexcept;

private:
	const uint64_t threshold_;
	const double probability_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE