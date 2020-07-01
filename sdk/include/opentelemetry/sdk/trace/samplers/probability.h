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

/** The default behavior is to apply the sampling probability only for Spans
  * that are root spans (no parent) and Spans with remote parent. However
  * there should be configuration to change this to "root spans only",
  * or "all spans".
  */
enum class SamplingBehavior
{
  // Root Spans (no parent) and Spans with a remote parent
  DETACHED_SPANS_ONLY,
  // Root Spans (no parent) only
  ROOT_SPANS_ONLY,
  // All Spans: Root Spans (no parent), Spans with a remote parent, and 
  // Spans with a direct parent
  ALL_SPANS
};

/**
 * The probability sampler, based on it's configuration, should either defer the
 * decision to sample to it's parent, or compute and return a decision based on
 * the provided trace_id and probability.
 */
class ProbabilitySampler : public Sampler
{
public:
  /**
   * @param probability a required value, 1 >= probability >= 0, that given any
   * random trace_id, ShouldSample will return RECORD_AND_SAMPLE
   * @param defer_parent an optional configuration setting, specifying whether
   * this sampler should defer to it's parent in cases where there is a 
   * parent SpanContext specified, and is not remote
   * @param sampling_behavior an optional configuration setting, specifying
   * what types of Spans should processed. all other types will return NOT_RECORD
   */
  explicit ProbabilitySampler(double probability, bool defer_parent = true,
                              SamplingBehavior sampling_behavior = SamplingBehavior::DETACHED_SPANS_ONLY);

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

  double GetProbability() const noexcept;

  const uint64_t threshold_;
  const double probability_;
  const bool defer_parent_;
  const SamplingBehavior sampling_behavior_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
