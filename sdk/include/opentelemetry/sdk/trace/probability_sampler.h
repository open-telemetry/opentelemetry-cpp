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
 * The probability sampler ...
 *
 */
class ProbabilitySampler : public Sampler
{
public:
  explicit ProbabilitySampler(double probability, bool defer_parent = true,
                              SamplingBehavior sampling_behavior = SamplingBehavior::DETACHED_SPANS_ONLY);

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
  const bool defer_parent_;
  const SamplingBehavior sampling_behavior_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
