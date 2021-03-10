#pragma once

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

#include "detail/hex.h"
#include "detail/string.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/text_map_propagator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

static const nostd::string_view kTraceHeader = "uber-trace-id";

template <typename T>
class JaegerPropagator : public TextMapPropagator<T>
{
public:
  using Getter = nostd::string_view (*)(const T &carrier, nostd::string_view trace_type);

  using Setter = void (*)(T &carrier,
                          nostd::string_view trace_type,
                          nostd::string_view trace_description);

  void Inject(Setter setter, T &carrier, const context::Context &context) noexcept override
  {
    SpanContext span_context = context.GetCurrentSpan();
    if (!span_context.IsValid())
    {
      return;
    }

    const size_t trace_id_length = 32;
    const size_t span_id_length  = 16;

    // trace-id(32):span-id(16):0:debug(2)
    char trace_identity[trace_id_length + span_id_length + 6];
    span_context.trace_id().ToLowerBase16({&trace_identity[0], trace_id_length});
    trace_identity[trace_id_length] = ':';
    span_context.span_id().ToLowerBase16({&trace_identity[trace_id_length + 1], span_id_length});
    trace_identity[trace_id_length + span_id_length + 1] = ':';
    trace_identity[trace_id_length + span_id_length + 2] = '0';
    trace_identity[trace_id_length + span_id_length + 3] = ':';
    trace_identity[trace_id_length + span_id_length + 4] = '0';
    trace_identity[trace_id_length + span_id_length + 5] = span_context.IsSampled() ? '1' : '0';

    setter(carrier, kTraceHeader, nostd::string_view(trace_identity, sizeof(trace_identity)));
  }

  context::Context Extract(Getter getter,
                           const T &carrier,
                           context::Context &context) noexcept override
  {
    SpanContext span_context = ExtractImpl(getter, carrier);
    nostd::shared_ptr<Span> sp{new DefaultSpan(span_context)};
    return context.SetValue(kSpanKey, sp);
  }

private:
  static TraceFlags GetTraceFlags(uint8_t jaeger_flags)
  {
    uint8_t sampled = jaeger_flags & 0x01;
    return TraceFlags(sampled);
  }

  static SpanContext ExtractImpl(Getter getter, const T &carrier)
  {
    nostd::string_view trace_identity = getter(carrier, kTraceHeader);

    const size_t trace_field_count = 4;
    nostd::string_view trace_fields[trace_field_count];

    if (detail::SplitString(trace_identity, ':', trace_fields, trace_field_count) != trace_field_count)
    {
      return SpanContext::GetInvalid();
    }

    nostd::string_view trace_id_hex = trace_fields[0];
    nostd::string_view span_id_hex  = trace_fields[1];
    nostd::string_view flags_hex    = trace_fields[3];

    if (!detail::IsValidHex(trace_id_hex) || !detail::IsValidHex(span_id_hex) ||
        !detail::IsValidHex(flags_hex))
    {
      return SpanContext::GetInvalid();
    }

    uint8_t trace_id[16];
    if (!detail::HexToBinary(trace_id_hex, trace_id, sizeof(trace_id)))
    {
      return SpanContext::GetInvalid();
    }

    uint8_t span_id[8];
    if (!detail::HexToBinary(span_id_hex, span_id, sizeof(span_id)))
    {
      return SpanContext::GetInvalid();
    }

    uint8_t flags;
    if (!detail::HexToBinary(flags_hex, &flags, sizeof(flags)))
    {
      return SpanContext::GetInvalid();
    }

    return SpanContext(TraceId(trace_id), SpanId(span_id), GetTraceFlags(flags), true);
  }
};

}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
