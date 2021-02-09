// Copyright 2020, OpenTelemetry Authors
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

#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/trace_state.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace trace_api = opentelemetry::trace;

/* SpanContext contains the state that must propagate to child Spans and across
 * process boundaries. It contains TraceId and SpanId, TraceFlags, TraceState
 * and whether it was propagated from a remote parent.
 */
class SpanContext final
{
public:
  /* A temporary constructor for an invalid SpanContext.
   * Trace id and span id are set to invalid (all zeros).
   *
   * @param sampled_flag a required parameter specifying if child spans should be
   * sampled
   * @param is_remote true if this context was propagated from a remote parent.
   */
  SpanContext(bool sampled_flag, bool is_remote)
      : trace_id_(),
        span_id_(),
        trace_flags_(trace_api::TraceFlags((uint8_t)sampled_flag)),
        is_remote_(is_remote),
        trace_state_(TraceState::GetDefault())
  {}

  SpanContext(TraceId trace_id,
              SpanId span_id,
              TraceFlags trace_flags,
              bool is_remote,
              TraceState trace_state = TraceState::GetDefault()) noexcept
      : trace_id_(trace_id),
        span_id_(span_id),
        trace_flags_(trace_flags),
        is_remote_(is_remote),
        trace_state_(trace_state)
  {}

  SpanContext(const SpanContext &ctx) = default;

  // @returns whether this context is valid
  bool IsValid() const noexcept { return trace_id_.IsValid() && span_id_.IsValid(); }

  // @returns the trace_flags associated with this span_context
  const trace_api::TraceFlags &trace_flags() const noexcept { return trace_flags_; }

  // @returns the trace_id associated with this span_context
  const trace_api::TraceId &trace_id() const noexcept { return trace_id_; }

  // @returns the span_id associated with this span_context
  const trace_api::SpanId &span_id() const noexcept { return span_id_; }

  // @returns the trace_state associated with this span_context
  const trace_api::TraceState &trace_state() const noexcept { return trace_state_; }

  SpanContext &operator=(const SpanContext &ctx) = default;

  bool IsRemote() const noexcept { return is_remote_; }

  static SpanContext GetInvalid() { return SpanContext(false, false); }

  bool IsSampled() const noexcept { return trace_flags_.IsSampled(); }

private:
  trace_api::TraceId trace_id_;
  trace_api::SpanId span_id_;
  trace_api::TraceFlags trace_flags_;
  trace_api::TraceState trace_state_;
  bool is_remote_ = false;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
