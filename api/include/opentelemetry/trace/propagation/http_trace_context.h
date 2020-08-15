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
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/trace/propagation/http_text_format.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_state.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
static const nostd::string_view kTraceParent = "traceparent";
static const nostd::string_view kTraceState  = "tracestate";
static const int kVersionBytes               = 2;
static const int kTraceIdBytes               = 32;
static const int kSpanIdBytes                = 16;
static const int kTraceFlagBytes             = 2;
static const int kTraceDelimiterBytes        = 3;
static const int kHeaderSize =
    kVersionBytes + kTraceIdBytes + kSpanIdBytes + kTraceFlagBytes + kTraceDelimiterBytes;
static const int kTraceStateMaxMembers    = 32;
static const int kHeaderElementLengths[4] = {2, 32, 16, 2};

// The HttpTraceContext provides methods to extract and inject
// context into headers of HTTP requests with traces.
// Example:
//    HttpTraceContext.inject(setter,&carrier,&context);
//    HttpTraceContext.extract(getter,&carrier,&context);
template <typename T>
class HttpTraceContext : public HTTPTextFormat<T>
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
    SpanContext span_context = GetCurrentSpan(context)->GetContext();
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
    std::cout<<"extraction begins"<<std::endl;
    SpanContext span_context    = ExtractImpl(getter, carrier);
    nostd::string_view span_key = "current-span";
    std::cout<<"context get"<<std::endl;
    nostd::shared_ptr<Span> sp{new DefaultSpan(span_context)};
    std::cout<<"set value"<<std::endl;
    return context.SetValue(span_key, sp);
  }

  static Span *GetCurrentSpan(const context::Context &context)
  {
    const nostd::string_view span_key = "current-span";
    context::Context ctx(context);
    nostd::shared_ptr<Span> span = nostd::get<nostd::shared_ptr<Span>>(ctx.GetValue(span_key));
    return (span.get());
  }

  static TraceId GenerateTraceIdFromString(nostd::string_view trace_id)
  {
    const char *trc_id = trace_id.begin();
    uint8_t buf[kTraceIdBytes / 2];
    int tmp;
    for (int i = 0; i < kTraceIdBytes; i++)
    {
      tmp = CharToInt(*trc_id);
      if (tmp < 0)
      {
        for (int j = 0; j < kTraceIdBytes / 2; j++)
        {
          buf[j] = 0;
        }
        return TraceId(buf);
      }
      if (i % 2 == 0)
      {
        buf[i / 2] = tmp * 16;
      }
      else
      {
        buf[i / 2] += tmp;
      }
      trc_id++;
    }
    return TraceId(buf);
  }

  static SpanId GenerateSpanIdFromString(nostd::string_view span_id)
  {
    const char *spn_id = span_id.begin();
    uint8_t buf[kSpanIdBytes / 2];
    int tmp;
    for (int i = 0; i < kSpanIdBytes; i++)
    {
      tmp = CharToInt(*spn_id);
      if (tmp < 0)
      {
        for (int j = 0; j < kSpanIdBytes / 2; j++)
        {
          buf[j] = 0;
        }
        return SpanId(buf);
      }
      if (i % 2 == 0)
      {
        buf[i / 2] = tmp * 16;
      }
      else
      {
        buf[i / 2] += tmp;
      }
      spn_id++;
    }
    return SpanId(buf);
  }

  static TraceFlags GenerateTraceFlagsFromString(nostd::string_view trace_flags)
  {
    uint8_t buf;
    int tmp1 = CharToInt(trace_flags[0]);
    int tmp2 = CharToInt(trace_flags[1]);
    if (tmp1 < 0 || tmp2 < 0)
      return TraceFlags(0);  // check for invalid char
    buf = tmp1 * 16 + tmp2;
    return TraceFlags(buf);
  }

private:
  static uint8_t CharToInt(char c)
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

  static void InjectTraceState(TraceState trace_state, T &carrier, Setter setter)
  {
    std::string trace_state_string = "";
    bool begin                     = true;
    for (const auto &entry : trace_state.Entries())
    {
      if (!begin)
      {
        trace_state_string += ",";
      }
      else
      {
        begin = !begin;
      }
      trace_state_string += std::string(entry.GetKey()) + "=" + std::string(entry.GetValue());
    }
    setter(carrier, kTraceState, trace_state_string);
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
      hex_string += trace_id[i];
    }
    hex_string += "-";
    for (int i = 0; i < 16; i++)
    {
      hex_string += span_id[i];
    }
    hex_string += "-";
    for (int i = 0; i < 2; i++)
    {
      hex_string += trace_flags[i];
    }
    setter(carrier, kTraceParent, hex_string);
  }

  static void InjectImpl(Setter setter, T &carrier, const SpanContext &span_context)
  {
    InjectTraceParent(span_context, carrier, setter);
    if (!span_context.trace_state().Empty())
    {
      InjectTraceState(span_context.trace_state(), carrier, setter);
    }
  }

  static SpanContext ExtractContextFromTraceParent(nostd::string_view trace_parent)
  {
    bool is_valid = trace_parent.length() == kHeaderSize && trace_parent[kVersionBytes] == '-' &&
                    trace_parent[kVersionBytes + kTraceIdBytes + 1] == '-' &&
                    trace_parent[kVersionBytes + kTraceIdBytes + kSpanIdBytes + 2] == '-';
    if (!is_valid)
    {
      std::cout << "Unparseable trace_parent header. Returning INVALID span context." << std::endl;
      return SpanContext(false, false);
    }
    nostd::string_view version;
    nostd::string_view trace_id;
    nostd::string_view span_id;
    nostd::string_view trace_flags;
    int elt_num   = 0;
    int countdown = kHeaderElementLengths[elt_num];
    int start_pos = -1;
    for (int i = 0; i < int(trace_parent.size()); i++)
    {
      if (trace_parent[i] == '\t')
        continue;
      else if (trace_parent[i] == '-')
      {
        if (countdown == 0)
        {
          if (elt_num == 0)
          {
            version = trace_parent.substr(start_pos, kHeaderElementLengths[elt_num]);
          }
          else if (elt_num == 1)
          {
            trace_id = trace_parent.substr(start_pos, kHeaderElementLengths[elt_num]);
          }
          else if (elt_num == 2)
          {
            span_id = trace_parent.substr(start_pos, kHeaderElementLengths[elt_num]);
          }
          else
          {
            return SpanContext(false,
                               false);  // Impossible to have more than 4 elements in parent header
          }
          countdown = kHeaderElementLengths[++elt_num];
          start_pos = -1;
        }
        else
        {
          return SpanContext(false, false);
        }
      }
      else if ((trace_parent[i] >= 'a' && trace_parent[i] <= 'f') ||
               (trace_parent[i] >= '0' && trace_parent[i] <= '9'))
      {
        if (start_pos == -1)
          start_pos = i;
        countdown--;
      }
      else
      {
        return SpanContext(false, false);
      }
    }
    trace_flags = trace_parent.substr(start_pos, kHeaderElementLengths[elt_num]);

    if (trace_id == "00000000000000000000000000000000" || span_id == "0000000000000000")
    {
      return SpanContext(false, false);
    }
    if (version == "ff")
    {
      return SpanContext(false, false);
    }
    if (trace_id.length() == 32 && span_id.length() == 16 && trace_flags.length() == 2)
    {
      TraceId trace_id_obj       = GenerateTraceIdFromString(trace_id);
      SpanId span_id_obj         = GenerateSpanIdFromString(span_id);
      TraceFlags trace_flags_obj = GenerateTraceFlagsFromString(trace_flags);
      return SpanContext(trace_id_obj, span_id_obj, trace_flags_obj, TraceState(), true);
    }
    else
    {
      std::cout << "Unparseable trace_parent header. Returning INVALID span context." << std::endl;
      return SpanContext(false, false);
    }
  }

  static TraceState ExtractTraceState(nostd::string_view &trace_state_header)
  {
    std::cout<<"1 it is"<<std::endl;
    std::cout<<"trace_state_header is "<<trace_state_header<<std::endl;
    TraceState trace_state = TraceState();
    int start_pos          = -1;
    int end_pos            = -1;
    int ctr_pos            = -1;
    int element_num        = 0;
    nostd::string_view key;
    nostd::string_view val;
    for (int i = 0; i < int(trace_state_header.length()); i++)
    {
      std::cout<<"i is "<<trace_state_header[i]<<" "<<i<<" th"<<std::endl;
      if (trace_state_header[i] == '\t')
        continue;
      else if (trace_state_header[i] == ',')
      {
        if (start_pos == -1 && end_pos == -1)
          continue;
        element_num++;
        if (ctr_pos != -1)
        {
          key = trace_state_header.substr(start_pos, ctr_pos - start_pos);
          val = trace_state_header.substr(ctr_pos + 1, end_pos - ctr_pos);
          std::cout<<"key: "<<key<<" val: "<<val<<std::endl;
          if (key != "") {
            trace_state.Set(key, val);
            nostd::string_view v;
            trace_state.Get(key,v);
            std::cout<<"value after setting is: "<<v<<std::endl;
          }
        }
        ctr_pos   = -1;
        end_pos   = -1;
        start_pos = -1;
      }
      else if (trace_state_header[i] == '=')
      {
        ctr_pos = i;
      }
      else
      {
        end_pos = i;
        if (start_pos == -1)
          start_pos = i;
      }
    }
    std::cout<<2<<std::endl;
    if (start_pos != -1 && end_pos != -1)
    {
      if (ctr_pos != -1)
      {
        key = trace_state_header.substr(start_pos, ctr_pos - start_pos + 1);
        val = trace_state_header.substr(ctr_pos + 1, end_pos - ctr_pos);
        if (key != "")
          trace_state.Set(key, val);
      }
      element_num++;
    }
    std::cout<<3<<std::endl;
    if (element_num >= kTraceStateMaxMembers)
    {
      std::cout<<"got into here?"<<std::endl;
      return TraceState();  // too many k-v pairs will result in an invalid trace state
    }
    std::cout<<"trace state returned"<<std::endl;
    return trace_state;
  }

  static void AddNewMember(TraceState &trace_state, nostd::string_view member)
  {
    for (int i = 0; i < int(member.length()); i++)
    {
      if (member[i] == '=')
      {
        trace_state.Set(member.substr(0, i), member.substr(i + 1, member.length() - i - 1));
        return;
      }
    }
  }

  static SpanContext ExtractImpl(Getter getter, const T &carrier)
  {
    nostd::string_view trace_parent = getter(carrier, kTraceParent);
    if (trace_parent == "")
    {
      return SpanContext(false, false);
    }
    SpanContext context_from_parent_header = ExtractContextFromTraceParent(trace_parent);
    if (!context_from_parent_header.IsValid())
    {
      return context_from_parent_header;
    }

    std::cout<<"trace parent complete"<<std::endl;
    nostd::string_view trace_state_header = getter(carrier, kTraceState);
    std::cout<<"getter complete"<<std::endl;
    if (trace_state_header == "" || trace_state_header.empty())
    {
      return context_from_parent_header;
    }

    TraceState trace_state = ExtractTraceState(trace_state_header);
    std::cout<<"trace state extract complete"<<std::endl;
    return SpanContext(context_from_parent_header.trace_id(), context_from_parent_header.span_id(),
                       context_from_parent_header.trace_flags(), trace_state, true);
  }
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
