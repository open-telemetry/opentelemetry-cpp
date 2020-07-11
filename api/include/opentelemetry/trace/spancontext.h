#pragma once

#include <cstdint>
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_state.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

public class SpanContext final:
    // The state of a Span to propagate between processes.
    // This class includes the immutable attributes of a :class:`.Span` that must
    // be propagated to a span's children and across process boundaries.
    public:
        TraceId trace_id_;
        SpanId span_id_;
        bool remote_;
        TraceFlags trace_flags_;
        TraceState trace_state_;
        static const SpanContext kInvalid = SpanContext(TraceId.getInvalid(),SpanId.getInvalid(),false,TraceFlags.getDefault(),TraceState.getDefault());

        SpanContext(TraceId trace_id, SpanId span_id, bool is_remote_, TraceFlags &trace_flags, TraceState &trace_state) {
            trace_id_ = trace_id;
            span_id_ = span_id;
            trace_flags_ = trace_flags;
            trace_state_ = trace_state;
            remote_ = is_remote_;
        }

        static SpanContext GetInvalid() {
            return kInvalid;
        }

        bool IsValid() {
            // Get whether this `SpanContext` is valid.
            // A `SpanContext` is said to be invalid if its trace ID or span ID is
            // invalid (i.e. ``0``).
            return trace_id_ != kInvalid_TRACE_ID && span_id_ != kInvalid_SPAN_ID;
        }

        TraceId GetTraceId() {
            return trace_id_;
        }

        SpanId GetSpanId() {
            return span_id_;
        }

        TraceFlags GetTraceFlags() {
            return trace_flags_;
        }

        TraceState GetTraceState() {
            return trace_state_;
        }

    private:
        static const nostd::string_view kInvalid_SPAN_ID = SpanId.fromLowerBase16("0000000000000000");
        static const nostd::string_view kInvalid_TRACE_ID = TraceId.fromLowerBase16("00000000000000000000000000000000");
}
OPENTELEMETRY_END_NAMESPACE