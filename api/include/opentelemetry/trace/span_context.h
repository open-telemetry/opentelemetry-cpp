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

namespace opentelemetry
{
namespace trace
{
namespace trace_api = opentelemetry::trace;

// SpanContext contains the state that must propagate to child Spans and across
// process boundaries. It contains the identifiers TraceId and SpanId,
// TraceFlags, TraceState, and whether it has a remote parent.
class SpanContext final
{
public:
  // An invalid SpanContext.
  SpanContext(bool sampled_flag, bool has_remote_parent) :
    trace_flags_(trace_api::TraceFlags((uint8_t) sampled_flag)), remote_parent_(has_remote_parent) {};

  const trace_api::TraceId &trace_id() const noexcept { return trace_id_; }
  const trace_api::SpanId &span_id() const noexcept { return span_id_; }
  const trace_api::TraceFlags &trace_flags() const noexcept { return trace_flags_; }

  bool IsValid() const noexcept { return trace_id_.IsValid() && span_id_.IsValid(); }

  bool IsSampled() const noexcept { return trace_flags_.IsSampled(); }
  bool HasRemoteParent() const noexcept { return remote_parent_; }

private:
  const trace_api::TraceId trace_id_;
  const trace_api::SpanId span_id_;
  const trace_api::TraceFlags trace_flags_;
  const bool remote_parent_ = false;
};
}  // namespace trace
}  // namespace opentelemetry
