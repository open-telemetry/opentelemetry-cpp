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
      : trace_flags_(trace::TraceFlags((uint8_t) false)), remote_parent_(false){};

  SpanContext(bool sampled_flag, bool has_remote_parent) noexcept
      : trace_flags_(trace::TraceFlags((uint8_t)sampled_flag)), remote_parent_(has_remote_parent){};
  SpanContext(TraceId trace_id,
              SpanId span_id,
              TraceFlags trace_flags,
              bool has_remote_parent) noexcept
  {
    trace_id_      = trace_id;
    span_id_       = span_id;
    trace_flags_   = trace_flags;
    remote_parent_ = has_remote_parent;
  }
  SpanContext(SpanContext &&ctx)
      : trace_id_(ctx.trace_id()), span_id_(ctx.span_id()), trace_flags_(ctx.trace_flags())
  {}
  SpanContext(const SpanContext &ctx)
      : trace_id_(ctx.trace_id()), span_id_(ctx.span_id()), trace_flags_(ctx.trace_flags())
  {}

  SpanContext &operator=(const SpanContext &ctx)
  {
    trace_id_    = ctx.trace_id_;
    span_id_     = ctx.span_id_;
    trace_flags_ = ctx.trace_flags_;
    return *this;
  };
  SpanContext &operator=(SpanContext &&ctx)
  {
    trace_id_    = ctx.trace_id_;
    span_id_     = ctx.span_id_;
    trace_flags_ = ctx.trace_flags_;
    return *this;
  };

  const TraceId &trace_id() const noexcept { return trace_id_; }
  const SpanId &span_id() const noexcept { return span_id_; }
  const TraceFlags &trace_flags() const noexcept { return trace_flags_; }

  bool IsValid() const noexcept { return trace_id_.IsValid() && span_id_.IsValid(); }

  bool HasRemoteParent() const noexcept { return remote_parent_; }

  static SpanContext GetInvalid() { return SpanContext(false, false); }

  bool IsSampled() const noexcept { return trace_flags_.IsSampled(); }

private:
  TraceId trace_id_;
  SpanId span_id_;
  TraceFlags trace_flags_;
  bool remote_parent_ = false;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE  // namespace opentelemetry
