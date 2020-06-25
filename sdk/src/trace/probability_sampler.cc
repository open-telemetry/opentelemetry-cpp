#include "opentelemetry/sdk/trace/probability_sampler.h"

#include <cmath>
#include <cstdint>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

ProbabilitySampler::ProbabilitySampler(double probability)
  : threshold_(CalculateThreshold(probability)), probability_(probability) {}

SamplingResult ProbabilitySampler::ShouldSample(
  const SpanContext *parent_context,
  trace_api::TraceId trace_id,
  nostd::string_view name,
  trace_api::SpanKind span_kind,
  const trace_api::KeyValueIterable &attributes) noexcept
{
  if (threshold_ == 0) return { Decision::NOT_RECORD, nullptr };

	if (parent_context == nullptr)
  {
	  if (CalculateThresholdFromBuffer(trace_id) <= threshold_)
    {
      return { Decision::RECORD_AND_SAMPLE, nullptr };
    }
	}
  else
  {
    if (parent_context->is_recording && parent_context->sampled_flag)
    {
      return { Decision::RECORD_AND_SAMPLE, nullptr };
    }
    else if (parent_context->is_recording)
    {
      return { Decision::RECORD, nullptr};
    }
	}

  return { Decision::NOT_RECORD, nullptr };
}

/**
 * @return Description MUST be ProbabilitySampler{0.000100}
 */
std::string ProbabilitySampler::GetDescription() const noexcept
{
	char buffer[30];
	sprintf(buffer, "ProbabilitySampler{%.6f}", GetProbability());
  return std::string(buffer);
}

uint64_t ProbabilitySampler::CalculateThreshold(double probability) const noexcept
{
  if (probability <= 0.0) return 0;
  if (probability >= 1.0) return UINT64_MAX;
  // We can't directly return probability * UINT64_MAX.
  //
  // UINT64_MAX is (2^64)-1, but as a double rounds up to 2^64.
  // For probabilities >= 1-(2^-54), the product wraps to zero!
  // Instead, calculate the high and low 32 bits separately.
  const double product = UINT32_MAX * probability;
  double hi_bits, lo_bits = ldexp(modf(product, &hi_bits), 32) + product;
  return (static_cast<uint64_t>(hi_bits) << 32) +
         static_cast<uint64_t>(lo_bits);
}

uint64_t ProbabilitySampler::CalculateThresholdFromBuffer(const trace_api::TraceId& trace_id) const noexcept
{
  nostd::span<const unsigned char, trace_id.kSize> buf = trace_id.Id();
  uint64_t res = 0;
  // We only use the first 8 bytes of TraceId.
  static_assert(trace_id.kSize >= 8, "TraceID must be at least 8 bytes long.");
  for (int i = 0; i < 8; ++i) {
    res |= (static_cast<uint64_t>(buf[i]) << (i * 8));
  }
  return res;
}

double ProbabilitySampler::GetProbability() const noexcept
{
  if (probability_ <= 0.0) return 0;
  if (probability_ >= 1.0) return 1.0;
  return probability_;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE