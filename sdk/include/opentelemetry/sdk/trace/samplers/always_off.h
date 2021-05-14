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
 * The always off sampler always returns DROP, effectively disabling
 * tracing functionality.
 */
class AlwaysOffSampler : public Sampler
{
public:
  /**
   * @return Returns DROP always
   */
  SamplingResult ShouldSample(
      const trace_api::SpanContext &parent_context,
      trace_api::TraceId /*trace_id*/,
      nostd::string_view /*name*/,
      trace_api::SpanKind /*span_kind*/,
      const opentelemetry::common::KeyValueIterable & /*attributes*/,
      const trace_api::SpanContextKeyValueIterable & /*links*/) noexcept override
  {
    if (!parent_context.IsValid())
    {
      return {Decision::DROP, nullptr, opentelemetry::trace::TraceState::GetDefault()};
    }
    else
    {
      return {Decision::DROP, nullptr, parent_context.trace_state()};
    }
  }

  /**
   * @return Description MUST be AlwaysOffSampler
   */
  nostd::string_view GetDescription() const noexcept override { return "AlwaysOffSampler"; }
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
