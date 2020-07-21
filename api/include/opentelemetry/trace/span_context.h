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

//#include <cstdint>
//#include <cstring>

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
  SpanContext() noexcept : trace_state_(new TraceState) {}
  SpanContext(SpanContext&& ctx) : trace_id_(ctx.trace_id()), span_id_(ctx.span_id()), trace_flags_(ctx.trace_flags()), trace_state_(nostd::unique_ptr(&(ctx.trace_state()))) {}
  // TODO
  //
  // static SpanContext Create(TraceId traceId, SpanId spanId, TraceFlags traceFlags, TraceState
  // traceState); static SpanContext CreateFromRemoteParent(...);

  const TraceId &trace_id() const noexcept { return trace_id_; }
  const SpanId &span_id() const noexcept { return span_id_; }
  const TraceFlags &trace_flags() const noexcept { return trace_flags_; }
  const TraceState &trace_state() const noexcept { return *trace_state_; }

  bool IsValid() const noexcept { return trace_id_.IsValid() && span_id_.IsValid(); }

  bool HasRemoteParent() const noexcept { return remote_parent_; }

  static SpanContext GetInvalid() { return SpanContext(); }

private:
  const TraceId trace_id_;
  const SpanId span_id_;
  const TraceFlags trace_flags_;
  const nostd::unique_ptr<TraceState> trace_state_;  // Never nullptr.
  const bool remote_parent_ = false;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE// namespace opentelemetry