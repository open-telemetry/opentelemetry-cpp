// Copyright 2020, Open Telemetry Authors
// Copyright 2017, OpenCensus Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "opentelemetry/sdk/trace/probability_sampler.h"

#include <cmath>
#include <cstdint>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

ProbabilitySampler::ProbabilitySampler(
  double probability, bool defer_parent, SamplingBehavior sampling_behavior 
)
: threshold_(CalculateThreshold(probability)), probability_(probability),
  defer_parent_(defer_parent), sampling_behavior_(sampling_behavior) {}

SamplingResult ProbabilitySampler::ShouldSample(
  const SpanContext *parent_context,
  trace_api::TraceId trace_id,
  nostd::string_view name,
  trace_api::SpanKind span_kind,
  const trace_api::KeyValueIterable &attributes) noexcept
{
  // Check if current Span should be sampled with current Sampling Behavior
  if (sampling_behavior_ == SamplingBehavior::DETACHED_SPANS_ONLY
      && parent_context != nullptr && !parent_context->is_remote) {
    return { Decision::NOT_RECORD, nullptr };
  }

  if (sampling_behavior_ == SamplingBehavior::ROOT_SPANS_ONLY
      && parent_context != nullptr) {
    return { Decision::NOT_RECORD, nullptr };
  }

  // Check if sampling decision should be defered to parent span, if applicable 
	if (parent_context == nullptr || !defer_parent_)
  {
    if (threshold_ == 0) return { Decision::NOT_RECORD, nullptr };

    // Calculcate probability based decision based on trace_id and threshold
	  if (CalculateThresholdFromBuffer(trace_id) <= threshold_)
    {
      return { Decision::RECORD_AND_SAMPLE, nullptr };
    }
	}
  else
  {
    // Return sampling decision based on parent span_context config
    if (parent_context->sampled)
    {
      return { Decision::RECORD_AND_SAMPLE, nullptr };
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
  // We only use the first 8 bytes of TraceId.
  static_assert(trace_id.kSize >= 8, "TraceID must be at least 8 bytes long.");

  uint8_t buf[trace_api::TraceId::kSize];
  
  for (int i = 0; i < 8; ++i) {
    buf[i] = trace_id.Id()[i];
  }

  uint64_t res = 0;
  
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
