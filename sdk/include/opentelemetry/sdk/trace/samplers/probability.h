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
    const SpanContext *parent_context,
    trace_api::TraceId trace_id,
    nostd::string_view /*name*/,
    trace_api::SpanKind /*span_kind*/,
    const trace_api::KeyValueIterable & /*attributes*/) noexcept override;

  /**
   * @return Description MUST be ProbabilitySampler{0.000100}
   */
  std::string GetDescription() const noexcept override;

private:
  /**
   * Converts a probability in [0, 1] to a threshold in [0, UINT64_MAX]
   *
   * @param probability a required value top be converted to uint64_t. is
   * bounded by 1 >= probability >= 0.
   * @return Returns threshold value computed after converting probability to
   * uint64_t datatype
   */
  uint64_t CalculateThreshold(double probability) const noexcept;

  /**
   * @param trace_id a required value to be converted to uint64_t. trace_id must
   * at least 8 bytes long
   * @return Returns threshold value computed after converting trace_id to
   * uint64_t datatype
   */
  uint64_t CalculateThresholdFromBuffer(const trace_api::TraceId& trace_id) const noexcept;

  std::string sampler_description_;
  const uint64_t threshold_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
