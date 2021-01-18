#pragma once

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
#include "opentelemetry/trace/propagation/http_text_format.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

static const nostd::string_view B3CombinedHeader = "b3";

static const nostd::string_view B3TraceIdHeader = "X-B3-TraceId";
static const nostd::string_view B3SpanIdHeader  = "X-B3-SpanId";
static const nostd::string_view B3SampledHeader = "X-B3-Sampled";

/*
     B3, single header:
                   b3: 80f198ee56343ba864fe8b2a57d3eff7-e457b5a2e4d86bd1-1-05e3ac9a4f6e3b90
                       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^ ^ ^^^^^^^^^^^^^^^^
                       0          TraceId            31 33  SpanId    48 | 52 ParentSpanId 68
                                                                        50 Debug flag
     Multiheader version:                                           X-B3-Sampled
                             X-B3-TraceId                X-B3-SpanId    X-B3-ParentSpanId (ignored)
*/

static const int kTraceIdHexStrLength   = 32;
static const int kSpanIdHexStrLength    = 16;
static const int kTraceFlagHexStrLength = 1;

// The B3PropagatorExtractor class provides an interface that enables extracting context from
// headers of HTTP requests. HTTP frameworks and clients can integrate with B3Propagator by
// providing the object  containing the headers, and a getter function for the extraction.
// Based on:
// https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/context/api-propagators.md#b3-extract
template <typename T>
class B3PropagatorExtractor : public HTTPTextFormat<T>
{
public:
  // Rules that manages how context will be extracted from carrier.
  using Getter = nostd::string_view (*)(const T &carrier, nostd::string_view trace_type);

  // Returns the context that is stored in the HTTP header carrier with the getter as extractor.
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
    uint8_t buf[kTraceIdHexStrLength / 2];
    uint8_t *b_ptr = buf;
    GenerateHexFromString(trace_id, kTraceIdHexStrLength, b_ptr);
    return TraceId(buf);
  }

  static SpanId GenerateSpanIdFromString(nostd::string_view span_id)
  {
    uint8_t buf[kSpanIdHexStrLength / 2];
    uint8_t *b_ptr = buf;
    GenerateHexFromString(span_id, kSpanIdHexStrLength, b_ptr);
    return SpanId(buf);
  }

  static TraceFlags GenerateTraceFlagsFromString(nostd::string_view trace_flags)
  {
    if (trace_flags.length() != 1 || (trace_flags[0] != '1' && trace_flags[0] != 'd'))
    {
      return TraceFlags(0);  // check for invalid length of flags
    }
    return TraceFlags(TraceFlags::kIsSampled);  // treat 'd' as kIsSampled
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

  static SpanContext ExtractImpl(Getter getter, const T &carrier)
  {
    // all these are hex values
    nostd::string_view trace_id;
    nostd::string_view span_id;
    nostd::string_view trace_flags;

    // first let's try a single-header variant
    auto singleB3Header = getter(carrier, B3CombinedHeader);
    if (singleB3Header != "")
    {
      trace_id = singleB3Header.substr(0, kTraceIdHexStrLength);
      if (singleB3Header.size() > 33)
      {
        span_id = singleB3Header.substr(33, kSpanIdHexStrLength);
      }
      if (singleB3Header.size() > 50)
      {
        trace_flags = singleB3Header.substr(50, kTraceFlagHexStrLength);
      }
    }
    else
    {
      trace_id    = getter(carrier, B3TraceIdHeader);
      span_id     = getter(carrier, B3SpanIdHeader);
      trace_flags = getter(carrier, B3SampledHeader);
    }

    // validate ids
    if (!IsValidHex(trace_id) || !IsValidHex(span_id) || !IsValidHex(trace_flags) ||
        trace_id == "00000000000000000000000000000000" || span_id == "0000000000000000")
    {
      return SpanContext(false, false);
    }

    // now convert hex to objects
    TraceId trace_id_obj       = GenerateTraceIdFromString(trace_id);
    SpanId span_id_obj         = GenerateSpanIdFromString(span_id);
    TraceFlags trace_flags_obj = GenerateTraceFlagsFromString(trace_flags);
    return SpanContext(trace_id_obj, span_id_obj, trace_flags_obj, true);
  }
};

// The B3Propagator class provides an interface that enables extracting and injecting
// context into headers of HTTP requests. HTTP frameworks and clients
// can integrate with B3Propagator by providing the object containing the
// headers, and a getter and setter function for the extraction and
// injection of values, respectively.
template <typename T>
class B3Propagator : public B3PropagatorExtractor<T>
{
public:
  // Rules that manages how context will be injected to carrier.
  using Setter = void (*)(T &carrier,
                          nostd::string_view trace_type,
                          nostd::string_view trace_description);
  // Sets the context for a HTTP header carrier with self defined rules.
  void Inject(Setter setter, T &carrier, const context::Context &context) noexcept override
  {
    SpanContext span_context = B3PropagatorExtractor<T>::GetCurrentSpan(context);
    if (!span_context.IsValid())
    {
      return;
    }
    char trace_id[32];
    TraceId(span_context.trace_id()).ToLowerBase16(trace_id);
    char span_id[16];
    SpanId(span_context.span_id()).ToLowerBase16(span_id);
    char trace_flags[2];
    TraceFlags(span_context.trace_flags()).ToLowerBase16(trace_flags);
    // Note: This is only temporary replacement for appendable string
    std::string hex_string = "";
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
    hex_string.push_back(trace_flags[1]);
    setter(carrier, B3CombinedHeader, hex_string);
  }
};

template <typename T>
class B3PropagatorMultiHeader : public B3PropagatorExtractor<T>
{
public:
  // Rules that manages how context will be injected to carrier.
  using Setter = void (*)(T &carrier,
                          nostd::string_view trace_type,
                          nostd::string_view trace_description);
  void Inject(Setter setter, T &carrier, const context::Context &context) noexcept override
  {
    SpanContext span_context = B3PropagatorExtractor<T>::GetCurrentSpan(context);
    if (!span_context.IsValid())
    {
      return;
    }
    char trace_id[32];
    TraceId(span_context.trace_id()).ToLowerBase16(trace_id);
    char span_id[16];
    SpanId(span_context.span_id()).ToLowerBase16(span_id);
    char trace_flags[2];
    TraceFlags(span_context.trace_flags()).ToLowerBase16(trace_flags);
    setter(carrier, B3TraceIdHeader, nostd::string_view(trace_id, sizeof(trace_id)));
    setter(carrier, B3SpanIdHeader, nostd::string_view(span_id, sizeof(span_id)));
    setter(carrier, B3SampledHeader, nostd::string_view(trace_flags + 1, 1));
  }
};

}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
