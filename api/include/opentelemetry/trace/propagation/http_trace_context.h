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

#include <string>
#include <map>
#include <iostream>
#include <array>
#include <exception>
#include "opentelemetry/trace/propagation/http_text_format.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/trace/default_span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
static const nostd::string_view kTraceParent = "traceparent";
static const nostd::string_view kTraceState = "tracestate";
// Parameters no longer needed because the toString functions are resolved else where
static const int kVersionBytes = 2;
static const int kTraceIdBytes = 32;
static const int kParentIdBytes = 16;
static const int kTraceFlagBytes = 2;
static const int kTraceDelimiterBytes = 3;
static const int kHeaderSize = kVersionBytes + kTraceIdBytes + kParentIdBytes + kTraceFlagBytes + kTraceDelimiterBytes;
static const int kTraceStateMaxMembers = 32;
static const nostd::string_view kTraceStateKeyValueDelimiter = "=";
static const int kHeaderElementLengths[4] = {2,32,16,2};
// The HttpTraceContext provides methods to extract and inject
// context into headers of HTTP requests with traces.
// Example:
//    HttpTraceContext.inject(setter,&carrier,&context);
//    HttpTraceContext.extract(getter,&carrier,&context);
template <typename T>
class HttpTraceContext : public HTTPTextFormat<T> {
    public:
        // Rules that manages how context will be extracted from carrier.
        using Getter = nostd::string_view(*)(const T &carrier, nostd::string_view trace_type);

        // Rules that manages how context will be injected to carrier.
        using Setter = void(*)(T &carrier, nostd::string_view trace_type,nostd::string_view trace_description);

        void Inject(Setter setter, T &carrier, const context::Context &context) override {
            SpanContext span_context = GetCurrentSpan(context)->GetContext();
            if (!span_context.IsValid()) {
                return;
            }
            InjectImpl(setter, carrier, span_context);
        }

        context::Context Extract(Getter getter, const T &carrier, context::Context &context) override {
            SpanContext span_context = ExtractImpl(getter,carrier);
            nostd::string_view span_key = "current-span";
            nostd::shared_ptr<Span> sp{new DefaultSpan(span_context)};
            return context.SetValue(span_key,sp);
        }

        static Span* GetCurrentSpan(const context::Context &context) {
            const nostd::string_view span_key = "current-span";
            context::Context ctx(context);
            nostd::shared_ptr<Span> span = nostd::get<nostd::shared_ptr<Span>>(ctx.GetValue(span_key));
            return (span.get());
        }

        static void InjectTraceParent(const SpanContext &span_context, T &carrier, Setter setter) {
            char trace_id[32];
            TraceId(span_context.trace_id()).ToLowerBase16(trace_id);
            char span_id[16];
            SpanId(span_context.span_id()).ToLowerBase16(span_id);
            char trace_flags[2];
            TraceFlags(span_context.trace_flags()).ToLowerBase16(trace_flags);
            // Note: This is only temporary replacement for appendable string
            std::string hex_string = "00-";
            for (int i = 0; i < 32; i++) {
                hex_string += trace_id[i];
            }
            hex_string += "-";
            for (int i = 0; i < 16; i++) {
                hex_string += span_id[i];
            }
            hex_string += "-";
            for (int i = 0; i < 2; i++) {
               hex_string += trace_flags[i];
            }
            setter(carrier, kTraceParent, hex_string);
        }

        static TraceId GenerateTraceIdFromString(nostd::string_view trace_id) {
            const char* tid = trace_id.begin();
            uint8_t buf[16];
            for (int i = 0; i < 32; i++)
            {
                if (i%2==0) {
                    buf[i/2] = CharToInt(*tid)*16;
                } else {
                    buf[i/2] += CharToInt(*tid);
                }
                tid++;
            }
            return TraceId(buf);
        }

        static SpanId GenerateSpanIdFromString(nostd::string_view span_id) {
            const char* sid = span_id.begin();
            uint8_t buf[8];
            for (int i = 0; i < 16; i++)
            {
                if (i%2==0) {
                    buf[i/2] = CharToInt(*sid)*16;
                } else {
                    buf[i/2] += CharToInt(*sid);
                }
                sid++;
            }
            return SpanId(buf);
        }

        static TraceFlags GenerateTraceFlagsFromString(nostd::string_view trace_flags) {
            uint8_t buf;
            buf = CharToInt(trace_flags[0])*16+CharToInt(trace_flags[1]);
            return TraceFlags(buf);
        }

        static void InjectTraceState(TraceState trace_state, T &carrier, Setter setter) {
            std::string trace_state_string = "";
            std::map<nostd::string_view,nostd::string_view> entries = trace_state.entries();
            for (std::map<nostd::string_view,nostd::string_view>::const_iterator it = entries.begin(); it != entries.end(); it++) {
                if (it != entries.begin()) trace_state_string += ",";
                trace_state_string += std::string(it->first) + "=" + std::string(it->second);
            }
            setter(carrier, kTraceState, trace_state_string);
        }

    private:
        static uint8_t CharToInt(char c) {
            if (c >= '0' && c <= '9') {
                return (int)(c - '0');
            } else if (c >= 'a' && c <= 'f') {
                return (int)(c - 'a' + 10);
            } else if (c >= 'A' && c <= 'F') {
                return (int)(c - 'A' + 10);
            } else {
                return 0;
            }
        }

        static void InjectImpl(Setter setter, T &carrier, const SpanContext &span_context) {
            InjectTraceParent(span_context, carrier, setter);
            if (!span_context.trace_state().empty()) {
                InjectTraceState(span_context.trace_state(), carrier, setter);
            }
        }

        static SpanContext ExtractContextFromTraceParent(nostd::string_view trace_parent) {
            bool is_valid = trace_parent.length() == kHeaderSize
                            && trace_parent[kVersionBytes] == '-'
                            && trace_parent[kVersionBytes+kTraceIdBytes+1] == '-'
                            && trace_parent[kVersionBytes+kTraceIdBytes+kParentIdBytes+2] == '-';
            if (!is_valid) {
                std::cout<<"Unparseable trace_parent header. Returning INVALID span context."<<std::endl;
                return SpanContext();
            }
            try {
                nostd::string_view version;
                nostd::string_view trace_id;
                nostd::string_view span_id;
                nostd::string_view trace_flags;
                int elt_num = 0;
                int countdown = kHeaderElementLengths[elt_num];
                int start_pos = -1;
                for (int i = 0; i < int(trace_parent.size()); i++) {
                    if (trace_parent[i]=='\t') continue;
                    else if (trace_parent[i]=='-') {
                        if (countdown==0) {
                            if (elt_num == 0) {
                                version = trace_parent.substr(start_pos,kHeaderElementLengths[elt_num]);
                            } else if (elt_num == 1) {
                                trace_id = trace_parent.substr(start_pos,kHeaderElementLengths[elt_num]);
                            } else if (elt_num == 2) {
                                span_id = trace_parent.substr(start_pos,kHeaderElementLengths[elt_num]);
                            } else {
                                throw; // Impossible to have more than 4 elements in parent header
                            }
                            countdown = kHeaderElementLengths[++elt_num];
                            start_pos = -1;
                        } else {
                            throw;
                        }
                    } else if ((trace_parent[i]>='a'&&trace_parent[i]<='f')||(trace_parent[i]>='0'&&trace_parent[i]<='9')) {
                        if (start_pos == -1) start_pos = i;
                        countdown--;
                    } else {
                        throw;
                    }
                }
                trace_flags = trace_parent.substr(start_pos,kHeaderElementLengths[elt_num]);

                if (trace_id == "00000000000000000000000000000000" || span_id == "0000000000000000") {
                      return SpanContext();
                }
                if (version == "ff") {
                      return SpanContext();
                }

                TraceId trace_id_obj = GenerateTraceIdFromString(trace_id);
                SpanId span_id_obj = GenerateSpanIdFromString(span_id);
                TraceFlags trace_flags_obj = GenerateTraceFlagsFromString(trace_flags);
                return SpanContext(trace_id_obj,span_id_obj,trace_flags_obj,TraceState(),true);
//                return SpanContext.CreateFromRemoteParent(trace_id_obj, span_id_obj, trace_flags_obj, TraceState());
            } catch (std::exception& e) {
                std::cout<<"Unparseable trace_parent header. Returning INVALID span context."<<std::endl;
                return SpanContext();
            }
        }

        static TraceState ExtractTraceState(nostd::string_view &trace_state_header) {
         //   TraceState.Builder trace_state_builder = TraceState.builder();
            TraceState trace_state = TraceState();
            int start_pos = -1;
            int end_pos = -1;
            int element_num = 0;
            nostd::string_view list_member;
            for (int i = 0; i < int(trace_state_header.length()); i++) {
                if (trace_state_header[i]=='\t') continue;
                else if (trace_state_header[i]==',') {
                    if (start_pos == -1 && end_pos == -1) continue;
                    element_num++;
                    list_member = trace_state_header.substr(start_pos,end_pos-start_pos+1);
                    if (list_member!="") AddNewMember(trace_state,list_member);
                    end_pos = -1;
                    start_pos = -1;
                } else {
                    end_pos = i;
                    if (start_pos==-1) start_pos = i;
                }
            }
            if (start_pos!=-1 && end_pos!=-1) {
                list_member = trace_state_header.substr(start_pos,end_pos-start_pos+1);
                if (list_member!="") AddNewMember(trace_state,list_member);
                element_num++;
            }

            if (element_num >= kTraceStateMaxMembers) {
                throw std::invalid_argument("TraceState has too many elements.");
            }
            return trace_state;
        }

        static void AddNewMember(TraceState &trace_state, nostd::string_view member) {
            for (int i = 0; i < int(member.length()); i++) {
                if (member[i] == '=') {
                    trace_state.Set(member.substr(0,i),member.substr(i+1,member.length()-i-1));
                    return;
                }
            }
        }

        static SpanContext ExtractImpl(Getter getter, const T &carrier) {
            nostd::string_view trace_parent = getter(carrier, kTraceParent);
            if (trace_parent == "") {
                return SpanContext();
            }
            SpanContext context_from_parent_header = ExtractContextFromTraceParent(trace_parent);
            if (!context_from_parent_header.IsValid()) {
                return context_from_parent_header;
            }

            nostd::string_view trace_state_header = getter(carrier, kTraceState);
//            trace_state_header = nostd::string_view(carrier[std::string(kTraceState)]);

            if (trace_state_header == "" || trace_state_header.empty()) {
                return context_from_parent_header;
            }

            try {
                TraceState trace_state = ExtractTraceState(trace_state_header);
                return SpanContext(
                    context_from_parent_header.trace_id(),
                    context_from_parent_header.span_id(),
                    context_from_parent_header.trace_flags(),
                    trace_state,
                    true
                );
//                return SpanContext.CreateFromRemoteParent(
//                    context_from_parent_header.GetTraceId(),
//                    context_from_parent_header.GetSpanId(),
//                    context_from_parent_header.GetTraceFlags(),
//                    trace_state
//                );
            } catch (std::exception& e) {
                std::cout<<"Unparseable tracestate header. Returning span context without state."<<std::endl;
                return context_from_parent_header;
//                return SpanContext.CreateFromRemoteParent(
//                    context_from_parent_header.GetTraceId(),
//                    context_from_parent_header.GetSpanId(),
//                    context_from_parent_header.GetTraceFlags(),
//                    TraceState.Builder().Build());
            }
        }
};
}
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
