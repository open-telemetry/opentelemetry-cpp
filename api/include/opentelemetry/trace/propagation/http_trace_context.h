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

#include "detail/context.h"
#include "detail/hex.h"
#include "detail/string.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/text_map_propagator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
static const nostd::string_view kTraceParent = "traceparent";
static const nostd::string_view kTraceState  = "tracestate";
static const size_t kTraceIdSize             = 32;
static const size_t kSpanIdSize              = 16;
static const size_t kTraceParentSize         = 55;

// The HttpTraceContext provides methods to extract and inject
// context into headers of HTTP requests with traces.
// Example:
//    HttpTraceContext().inject(setter, carrier, context);
//    HttpTraceContext().extract(getter, carrier, context);
template <typename T>
class HttpTraceContext : public TextMapPropagator<T>
{
public:
  // Rules that manages how context will be extracted from carrier.
  using Getter = nostd::string_view (*)(const T &carrier, nostd::string_view trace_type);

  // Rules that manages how context will be injected to carrier.
  using Setter = void (*)(T &carrier,
                          nostd::string_view trace_type,
                          nostd::string_view trace_description);

  void Inject(Setter setter, T &carrier, const context::Context &context) noexcept override
  {
    SpanContext span_context = detail::GetCurrentSpan(context);
    if (!span_context.IsValid())
    {
      return;
    }
    InjectImpl(setter, carrier, span_context);
  }

  context::Context Extract(Getter getter,
                           const T &carrier,
                           context::Context &context) noexcept override
  {
    SpanContext span_context = ExtractImpl(getter, carrier);
    nostd::shared_ptr<Span> sp{new DefaultSpan(span_context)};
    return context.SetValue(kSpanKey, sp);
  }

  static TraceId TraceIdFromHex(nostd::string_view trace_id)
  {
    uint8_t buf[kTraceIdSize / 2];
    detail::HexToBinary(trace_id, buf, sizeof(buf));
    return TraceId(buf);
  }

  static SpanId SpanIdFromHex(nostd::string_view span_id)
  {
    uint8_t buf[kSpanIdSize / 2];
    detail::HexToBinary(span_id, buf, sizeof(buf));
    return SpanId(buf);
  }

  static TraceFlags TraceFlagsFromHex(nostd::string_view trace_flags)
  {
    uint8_t flags;
    detail::HexToBinary(trace_flags, &flags, sizeof(flags));
    return TraceFlags(flags);
  }

private:
  static void InjectImpl(Setter setter, T &carrier, const SpanContext &span_context)
  {
    char trace_parent[kTraceParentSize];
    trace_parent[0] = '0';
    trace_parent[1] = '0';
    trace_parent[2] = '-';
    span_context.trace_id().ToLowerBase16({&trace_parent[3], kTraceIdSize});
    trace_parent[kTraceIdSize + 3] = '-';
    span_context.span_id().ToLowerBase16({&trace_parent[kTraceIdSize + 4], kSpanIdSize});
    trace_parent[kTraceIdSize + kSpanIdSize + 4] = '-';
    span_context.trace_flags().ToLowerBase16({&trace_parent[kTraceIdSize + kSpanIdSize + 5], 2});

    setter(carrier, kTraceParent, nostd::string_view(trace_parent, sizeof(trace_parent)));
    setter(carrier, kTraceState, span_context.trace_state()->ToHeader());
  }

  static SpanContext ExtractContextFromTraceHeaders(nostd::string_view trace_parent,
                                                    nostd::string_view trace_state)
  {
    if (trace_parent.size() < kTraceParentSize)
    {
      return SpanContext::GetInvalid();
    }

    std::array<nostd::string_view, 4> fields{};
    if (detail::SplitString(trace_parent, '-', fields.data(), 4) != 4)
    {
      return SpanContext::GetInvalid();
    }

    nostd::string_view version_hex     = fields[0];
    nostd::string_view trace_id_hex    = fields[1];
    nostd::string_view span_id_hex     = fields[2];
    nostd::string_view trace_flags_hex = fields[3];

    if (!detail::IsValidHex(version_hex) || !detail::IsValidHex(trace_id_hex) ||
        !detail::IsValidHex(span_id_hex) || !detail::IsValidHex(trace_flags_hex))
    {
      return SpanContext::GetInvalid();
    }

    TraceId trace_id       = TraceIdFromHex(trace_id_hex);
    SpanId span_id         = SpanIdFromHex(span_id_hex);
    TraceFlags trace_flags = TraceFlagsFromHex(trace_flags_hex);

    if (!trace_id.IsValid() || !span_id.IsValid() || !trace_flags.IsValid())
    {
      return SpanContext::GetInvalid();
    }

    return SpanContext(trace_id, span_id, trace_flags, true,
                       opentelemetry::trace::TraceState::FromHeader(trace_state));
  }

  static SpanContext ExtractImpl(Getter getter, const T &carrier)
  {
    nostd::string_view trace_parent = getter(carrier, kTraceParent);
    nostd::string_view trace_state  = getter(carrier, kTraceState);
    if (trace_parent == "")
    {
      return SpanContext::GetInvalid();
    }

    return ExtractContextFromTraceHeaders(trace_parent, trace_state);
  }
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
