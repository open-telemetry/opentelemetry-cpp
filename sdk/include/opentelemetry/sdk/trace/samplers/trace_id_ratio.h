// Copyright 2021, OpenTelemetry Authors
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
  SamplingResult ShouldSample(
      const trace_api::SpanContext & /*parent_context*/,
      trace_api::TraceId trace_id,
      nostd::string_view /*name*/,
      trace_api::SpanKind /*span_kind*/,
      const opentelemetry::common::KeyValueIterable & /*attributes*/,
      const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept override;

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
