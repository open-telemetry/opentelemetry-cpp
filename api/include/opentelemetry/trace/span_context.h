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
// SpanContext contains the state that must propagate to child Spans and across
// process boundaries. It contains the identifiers TraceId and SpanId,
// TraceFlags, TraceState, and whether it has a remote parent.
class SpanContext final
{
public:
  // An invalid SpanContext.
  SpanContext() noexcept
      : trace_flags_(new trace::TraceFlags((uint8_t) false)),
        trace_state_(new TraceState),
        remote_parent_(false){};

  SpanContext(bool sampled_flag, bool has_remote_parent) noexcept
      : trace_flags_(new trace::TraceFlags((uint8_t)sampled_flag)),
        trace_state_(new TraceState),
        remote_parent_(has_remote_parent){};
  SpanContext(TraceId trace_id,
              SpanId span_id,
              TraceFlags trace_flags,
              TraceState trace_state,
              bool has_remote_parent) noexcept
  {
    trace_id_.reset(&trace_id);
    span_id_.reset(&span_id);
    trace_flags_.reset(&trace_flags);
    trace_state_.reset(&trace_state);
    remote_parent_ = has_remote_parent;
  }
  SpanContext(SpanContext &&ctx)
      : trace_id_(ctx.trace_id_.get()),
        span_id_(ctx.span_id_.get()),
        trace_flags_(ctx.trace_flags_.get()),
        trace_state_(ctx.trace_state_.get())
  {}
  SpanContext(const SpanContext &ctx)
      : trace_id_(ctx.trace_id_.get()),
        span_id_(ctx.span_id_.get()),
        trace_flags_(ctx.trace_flags_.get()),
        trace_state_(ctx.trace_state_.get())
  {}

  SpanContext &operator=(const SpanContext &ctx)
  {
    trace_id_.reset(ctx.trace_id_.get());
    span_id_.reset(ctx.span_id_.get());
    trace_flags_.reset(ctx.trace_flags_.get());
    trace_state_.reset(ctx.trace_state_.get());
    return *this;
  };
  SpanContext &operator=(SpanContext &&ctx)
  {
    trace_id_.reset(ctx.trace_id_.get());
    span_id_.reset(ctx.span_id_.get());
    trace_flags_.reset(ctx.trace_flags_.get());
    trace_state_.reset(ctx.trace_state_.get());
    return *this;
  };

  const TraceId &trace_id() const noexcept { return *(trace_id_.get()); }
  const SpanId &span_id() const noexcept { return *(span_id_.get()); }
  const TraceFlags &trace_flags() const noexcept { return *(trace_flags_.get()); }
  const TraceState &trace_state() const noexcept { return *(trace_state_.get()); }

  bool IsValid() const noexcept { return trace_id_.get()->IsValid() && span_id_.get()->IsValid(); }

  bool HasRemoteParent() const noexcept { return remote_parent_; }

  static SpanContext GetInvalid() { return SpanContext(false, false); }

  static SpanContext GetRandom() {
    return SpanContext(TraceId::GetRandom(), SpanId::GetRandom(), TraceFlags::GetRandom(), true);
  }

  static SpanContext UpdateSpanId(SpanContext span_context) {
    return SpanContext(span_context.trace_id(), SpanId::GetRandom(), TraceFlags::GetRandom(), span_context.HasRemoteParent());
  }

  bool IsSampled() const noexcept { return trace_flags_.IsSampled(); }

private:
  nostd::unique_ptr<TraceId> trace_id_;
  nostd::unique_ptr<SpanId> span_id_;
  nostd::unique_ptr<TraceFlags> trace_flags_;
  nostd::unique_ptr<TraceState> trace_state_;  // Never nullptr.
  bool remote_parent_ = false;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
