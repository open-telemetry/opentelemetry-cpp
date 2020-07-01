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

#include "opentelemetry/sdk/trace/samplers/probability.h"

#include <cmath>
#include <cstdint>
#include <stdexcept>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

ProbabilitySampler::ProbabilitySampler(double probability)
: threshold_(CalculateThreshold(probability))
{
    if (probability > 1.0 || probability < 0) {
      throw std::invalid_argument("Invalid value received for probability. Must fall within bounds [1, 0].");
    }

    char buffer[30];
    sprintf(buffer, "ProbabilitySampler{%.6f}", probability);
    sampler_description_ = std::string(buffer);
  }

SamplingResult ProbabilitySampler::ShouldSample(
  const SpanContext *parent_context,
  trace_api::TraceId trace_id,
  nostd::string_view name,
  trace_api::SpanKind span_kind,
  const trace_api::KeyValueIterable &attributes) noexcept
{
  if (parent_context && !parent_context->is_remote) {
    if (parent_context->sampled) {
      return { Decision::RECORD_AND_SAMPLE, nullptr };
    } else {
      return { Decision::NOT_RECORD, nullptr };
    }
  }

  if (threshold_ == 0) return { Decision::NOT_RECORD, nullptr };

  if (CalculateThresholdFromBuffer(trace_id) <= threshold_)
  {
    return { Decision::RECORD_AND_SAMPLE, nullptr };
  }

  return { Decision::NOT_RECORD, nullptr };
}

std::string ProbabilitySampler::GetDescription() const noexcept
{
  return sampler_description_;
}

uint64_t ProbabilitySampler::CalculateThreshold(double probability) const noexcept
{
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
  static_assert(trace_api::TraceId::kSize >= 8, "TraceID must be at least 8 bytes long.");

  uint64_t res = 0;
  std::memcpy(&res, &trace_id, 8);
  
  return res;
}
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
