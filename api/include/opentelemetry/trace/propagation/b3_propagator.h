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

static const nostd::string_view kB3CombinedHeader = "b3";

static const nostd::string_view kB3TraceIdHeader = "X-B3-TraceId";
static const nostd::string_view kB3SpanIdHeader  = "X-B3-SpanId";
static const nostd::string_view kB3SampledHeader = "X-B3-Sampled";

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
// providing the object containing the headers, and a getter function for the extraction. Based on:
// https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/context/api-propagators.md#b3-extract
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
    GenerateBuffFromHexStrPad0(trace_id, sizeof(buf), buf);
    return TraceId(buf);
  }

  static SpanId GenerateSpanIdFromString(nostd::string_view span_id)
  {
    uint8_t buf[kSpanIdHexStrLength / 2];
    GenerateBuffFromHexStrPad0(span_id, sizeof(buf), buf);
    return SpanId(buf);
  }

  static TraceFlags GenerateTraceFlagsFromString(nostd::string_view trace_flags)
  {
    if (trace_flags.length() != 1 || (trace_flags[0] != '1' && trace_flags[0] != 'd'))
    {  // check for invalid length of flags and treat 'd' as sampled
      return TraceFlags(0);
    }
    return TraceFlags(TraceFlags::kIsSampled);
  }

private:
  // Converts hex numbers (string_view) into bytes stored in a buffer and pads buffer with 0.
  static void GenerateBuffFromHexStrPad0(nostd::string_view hexStr, int bufSize, uint8_t *buf)
  {  // we are doing this starting from "right" side for left-padding
    nostd::string_view::size_type posInp = hexStr.length();
    int posOut                           = bufSize;
    while (posOut--)
    {
      int val = 0;
      if (posInp)
      {
        int hexDigit2 = HexToInt(hexStr[--posInp]);  // low nibble
        int hexDigit1 = 0;
        if (posInp)
        {
          hexDigit1 = HexToInt(hexStr[--posInp]);
        }
        if (hexDigit1 < 0 || hexDigit2 < 0)
        {  // malformed hex sequence. Fill entire buffer with zeroes.
          for (int j = 0; j < bufSize; j++)
          {
            buf[j] = 0;
          }
          return;
        }
        val = hexDigit1 * 16 + hexDigit2;
      }
      buf[posOut] = val;
    }
  }

  // Converts a single character to a corresponding integer (e.g. '1' to 1), return -1
  // if the character is not a valid number in hex.
  static int8_t HexToInt(char c)
  {
    if (c >= '0' && c <= '9')
    {
      return (int8_t)(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
      return (int8_t)(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
      return (int8_t)(c - 'A' + 10);
    }
    else
    {
      return -1;
    }
  }

  static SpanContext ExtractImpl(Getter getter, const T &carrier)
  {
    // all these are hex values
    nostd::string_view trace_id;
    nostd::string_view span_id;
    nostd::string_view trace_flags;

    // first let's try a single-header variant
    auto singleB3Header = getter(carrier, kB3CombinedHeader);
    if (!singleB3Header.empty())
    {
      // From: https://github.com/openzipkin/b3-propagation/blob/master/RATIONALE.md
      // trace_id can be 16 or 32 chars
      auto firstSep = singleB3Header.find('-');
      trace_id      = singleB3Header.substr(0, firstSep);
      if (firstSep != nostd::string_view::npos)
      {  // at least two fields are required
        auto secondSep = singleB3Header.find('-', firstSep + 1);
        if (secondSep != nostd::string_view::npos)
        {  // more than two fields - check also trace_flags
          span_id = singleB3Header.substr(firstSep + 1, secondSep - firstSep - 1);
          if (secondSep + 1 < singleB3Header.size())
          {
            trace_flags = singleB3Header.substr(secondSep + 1, kTraceFlagHexStrLength);
          }
        }
        else
        {
          span_id = singleB3Header.substr(firstSep + 1);
        }
      }
    }
    else
    {
      trace_id    = getter(carrier, kB3TraceIdHeader);
      span_id     = getter(carrier, kB3SpanIdHeader);
      trace_flags = getter(carrier, kB3SampledHeader);
    }

    // now convert hex to objects
    TraceId trace_id_obj = GenerateTraceIdFromString(trace_id);
    SpanId span_id_obj   = GenerateSpanIdFromString(span_id);
    if (!trace_id_obj.IsValid() || !span_id_obj.IsValid())
    {
      return SpanContext(false, false);
    }
    TraceFlags trace_flags_obj = GenerateTraceFlagsFromString(trace_flags);
    return SpanContext(trace_id_obj, span_id_obj, trace_flags_obj, true);
  }
};

// The B3Propagator class provides interface that enables extracting and injecting context into
// single header of HTTP Request.
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
    char trace_id[kTraceIdHexStrLength];
    TraceId(span_context.trace_id()).ToLowerBase16(trace_id);
    char span_id[kSpanIdHexStrLength];
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
    setter(carrier, kB3CombinedHeader, hex_string);
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
    setter(carrier, kB3TraceIdHeader, nostd::string_view(trace_id, sizeof(trace_id)));
    setter(carrier, kB3SpanIdHeader, nostd::string_view(span_id, sizeof(span_id)));
    setter(carrier, kB3SampledHeader, nostd::string_view(trace_flags + 1, 1));
  }
};

}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
