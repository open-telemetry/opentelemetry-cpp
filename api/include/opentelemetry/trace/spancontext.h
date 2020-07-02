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

public class SpanContext:
    // The state of a Span to propagate between processes.
    // This class includes the immutable attributes of a :class:`.Span` that must
    // be propagated to a span's children and across process boundaries.
    public:
        TraceId traceId;
        SpanId spanId;
        bool remote;
        TraceFlags traceFlags;
        TraceState traceState;
        static SpanContext INVALID = SpanContext(TraceId.getInvalid(),SpanId.getInvalid(),false,TraceFlags.getDefault(),TraceState.getDefault());

        SpanContext(TraceId trace_id, SpanId span_id, bool is_remote, TraceFlags &trace_flags, TraceState &trace_state) {
            traceId = trace_id;
            spanId = span_id;
            traceFlags = trace_flags;
            traceState = trace_state;
            remote = is_remote;
        }

        static SpanContext getInvalid() {
            return INVALID;
        }

        bool isValid() {
            // Get whether this `SpanContext` is valid.
            // A `SpanContext` is said to be invalid if its trace ID or span ID is
            // invalid (i.e. ``0``).
            return traceId != INVALID_TRACE_ID && self.span_id != INVALID_SPAN_ID;
        }

        TraceId getTraceId() {
            return traceId;
        }

        SpanId getSpanId() {
            return spanId;
        }

        TraceFlags getTraceFlags() {
            return traceFlags;
        }

        TraceState getTraceState() {
            return traceState;
        }
}
OPENTELEMETRY_END_NAMESPACE