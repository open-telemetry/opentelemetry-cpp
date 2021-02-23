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

#include <array>
#include <iostream>
#include <map>
#include <string>
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/text_map_propagator.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
static const nostd::string_view kTraceParent = "traceparent";
static const nostd::string_view kTraceState  = "tracestate";
static const int kTraceDelimiterBytes        = 3;
static const int kHeaderElementLengths[4]    = {
    2, 32, 16, 2};  // 0: version, 1: trace id, 2: span id, 3: trace flags
static const int kHeaderSize = kHeaderElementLengths[0] + kHeaderElementLengths[1] +
                               kHeaderElementLengths[2] + kHeaderElementLengths[3] +
                               kTraceDelimiterBytes;
static const int kTraceStateMaxMembers = 32;
static const int kVersionBytes         = 2;
static const int kTraceIdBytes         = 32;
static const int kSpanIdBytes          = 16;
static const int kTraceFlagBytes       = 2;

// The HttpTraceContext provides methods to extract and inject
// context into headers of HTTP requests with traces.
// Example:
//    HttpTraceContext.inject(setter,&carrier,&context);
//    HttpTraceContext.extract(getter,&carrier,&context);
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
    SpanContext span_context = GetCurrentSpan(context);
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

  static SpanContext GetCurrentSpan(const context::Context &context)
  {
    context::Context ctx(context);
    context::ContextValue span = ctx.GetValue(kSpanKey);
    if (nostd::holds_alternative<nostd::shared_ptr<Span>>(span))
    {
      return nostd::get<nostd::shared_ptr<Span>>(span).get()->GetContext();
    }
    return SpanContext::GetInvalid();
  }

  static TraceId GenerateTraceIdFromString(nostd::string_view trace_id)
  {
    int trace_id_len = kHeaderElementLengths[1];
    uint8_t buf[kTraceIdBytes / 2];
    uint8_t *b_ptr = buf;
    GenerateHexFromString(trace_id, trace_id_len, b_ptr);
    return TraceId(buf);
  }

  static SpanId GenerateSpanIdFromString(nostd::string_view span_id)
  {
    int span_id_len = kHeaderElementLengths[2];
    uint8_t buf[kSpanIdBytes / 2];
    uint8_t *b_ptr = buf;
    GenerateHexFromString(span_id, span_id_len, b_ptr);
    return SpanId(buf);
  }

  static TraceFlags GenerateTraceFlagsFromString(nostd::string_view trace_flags)
  {
    if (trace_flags.length() > 2)
    {
      return TraceFlags(0);  // check for invalid length of flags
    }
    int tmp1 = HexToInt(trace_flags[0]);
    int tmp2 = HexToInt(trace_flags[1]);
    if (tmp1 < 0 || tmp2 < 0)
      return TraceFlags(0);  // check for invalid char
    uint8_t buf = tmp1 * 16 + tmp2;
    return TraceFlags(buf);
  }

private:
  // Converts the hex numbers stored as strings into bytes stored in a buffer.
  static void GenerateHexFromString(nostd::string_view string, int bytes, uint8_t *buf)
  {
    const char *str_id = string.data();
    for (int i = 0; i < bytes; i++)
    {
      int tmp = HexToInt(str_id[i]);
      if (tmp < 0)
      {
        for (int j = 0; j < bytes / 2; j++)
        {
          buf[j] = 0;
        }
        return;
      }
      if (i % 2 == 0)
      {
        buf[i / 2] = tmp * 16;
      }
      else
      {
        buf[i / 2] += tmp;
      }
    }
  }

  // Converts a single character to a corresponding integer (e.g. '1' to 1), return -1
  // if the character is not a valid number in hex.
  static uint8_t HexToInt(char c)
  {
    if (c >= '0' && c <= '9')
    {
      return (int)(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
      return (int)(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
      return (int)(c - 'A' + 10);
    }
    else
    {
      return -1;
    }
  }

  static void InjectTraceParent(const SpanContext &span_context, T &carrier, Setter setter)
  {
    char trace_id[32];
    TraceId(span_context.trace_id()).ToLowerBase16(trace_id);
    char span_id[16];
    SpanId(span_context.span_id()).ToLowerBase16(span_id);
    char trace_flags[2];
    TraceFlags(span_context.trace_flags()).ToLowerBase16(trace_flags);
    // Note: This is only temporary replacement for appendable string
    std::string hex_string = "00-";
    for (int i = 0; i < 32; i++)
    {
      hex_string.push_back(trace_id[i]);
    }
    hex_string.push_back('-');
    for (int i = 0; i < 16; i++)
    {
      hex_string.push_back(span_id[i]);
    }
    hex_string.push_back('-');
    for (int i = 0; i < 2; i++)
    {
      hex_string.push_back(trace_flags[i]);
    }
    setter(carrier, kTraceParent, hex_string);
  }

  static void InjectImpl(Setter setter, T &carrier, const SpanContext &span_context)
  {
    InjectTraceParent(span_context, carrier, setter);
  }

  static bool IsValidHex(nostd::string_view string_view)
  {
    for (int i = 0; i < string_view.length(); i++)
    {
      if (!(string_view[i] >= '0' && string_view[i] <= '9') &&
          !(string_view[i] >= 'a' && string_view[i] <= 'f'))
        return false;
    }
    return true;
  }

  static SpanContext ExtractContextFromTraceParent(nostd::string_view trace_parent)
  {
    if (trace_parent.length() != kHeaderSize || trace_parent[kHeaderElementLengths[0]] != '-' ||
        trace_parent[kHeaderElementLengths[0] + kHeaderElementLengths[1] + 1] != '-' ||
        trace_parent[kHeaderElementLengths[0] + kHeaderElementLengths[1] +
                     kHeaderElementLengths[2] + 2] != '-')
    {
      std::cout << "Unparseable trace_parent header. Returning INVALID span context." << std::endl;
      return SpanContext(false, false);
    }
    nostd::string_view version = trace_parent.substr(0, kHeaderElementLengths[0]);
    nostd::string_view trace_id =
        trace_parent.substr(kHeaderElementLengths[0] + 1, kHeaderElementLengths[1]);
    nostd::string_view span_id = trace_parent.substr(
        kHeaderElementLengths[0] + kHeaderElementLengths[1] + 2, kHeaderElementLengths[2]);
    nostd::string_view trace_flags = trace_parent.substr(
        kHeaderElementLengths[0] + kHeaderElementLengths[1] + kHeaderElementLengths[2] + 3);

    if (version == "ff" || trace_id == "00000000000000000000000000000000" ||
        span_id == "0000000000000000")
    {
      return SpanContext(false, false);
    }

    // validate ids
    if (!IsValidHex(version) || !IsValidHex(trace_id) || !IsValidHex(span_id) ||
        !IsValidHex(trace_flags))
    {
      return SpanContext(false, false);
    }

    TraceId trace_id_obj       = GenerateTraceIdFromString(trace_id);
    SpanId span_id_obj         = GenerateSpanIdFromString(span_id);
    TraceFlags trace_flags_obj = GenerateTraceFlagsFromString(trace_flags);
    return SpanContext(trace_id_obj, span_id_obj, trace_flags_obj, true);
  }

  static SpanContext ExtractImpl(Getter getter, const T &carrier)
  {
    nostd::string_view trace_parent = getter(carrier, kTraceParent);
    if (trace_parent == "")
    {
      return SpanContext(false, false);
    }

    return ExtractContextFromTraceParent(trace_parent);
  }
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
