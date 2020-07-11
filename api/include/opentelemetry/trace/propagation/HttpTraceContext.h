#include <vector>
#include <exception>
#include "opentelemetry/trace/propagation/httptextformat.h"
#include "opentelemetry/trace/spancontext.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/default_span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
namespace
{

static Context SetSpanInContext(Span &span, Context &context) {
    Context new_values = Context(context);
    // I don't know if the SPAN_KEY is defined in the context.h.
    // My point is that since each key when it is created is unique in terms of its id even though they may have the same name,
    // it would make sense to define those keys in a single file only and had to be referenced by other files to store and retrieve values,
    // otherwise I will not be able to access any fields, for example, "current-span" as CreateKey("current-span") will
    // not work because the id is different when the value is put into despite the Key is also created from
    // CreateKey("current-span").
    // Don't know if I get the correct understanding there.
    new_values.setValue(Context.SPAN_KEY,span);
    return new_values;
}

static Span GetCurrentSpan(Context &context) {
    Span span = context.GetValue(Context.kSpanKey);
    if (span == NULL) {
        return NULL;
    }
    return span;
}

// The HttpTraceContext provides methods to extract and inject
// context into headers of HTTP requests with traces.
// Example:
//    HttpTraceContext.inject(setter,&carrier,&context);
//    HttpTraceContext.extract(getter,&carrier,&context);
class HttpTraceContext : public HTTPTextFormat
{
    public:
        List<nostd::string_view> Fields() {
            static const auto* kFields = new std::vector<nostd::string_view>({kTraceParent, kTraceState});
            return kFields;
        }

        void Inject(Setter setter, T &carrier, const Context &context) override {
            common::AttributeValue span = GetCurrentSpan(context);
            if (span == NULL || !span.GetContext().IsValid()) {
                // We don't have span.getContext() in span.h, should we just use span? As well as acquiring validity. (I do know how to implement them though)
                return;
            }
            InjectImpl(setter, carrier, span.GetContext());
        }

        Context Extract(Getter getter, const T &carrier, Context &context) override {
            SpanContext span_context = ExtractImpl(carrier, getter);
            return SetSpanInContext(trace.DefaultSpan(span_context), context);
        }

    private:
        static const nostd::string_view kTraceParent = "traceparent";
        static const nostd::string_view kTraceState = "tracestate";
        static const int kVersionBytes = 2;
        static const int kTraceIdBytes = 32;
        static const int kParentIdBytes = 16;
        static const int kTraceFlagBytes = 2;
        static const int kTraceDelimiterBytes = 3;
        static const int kHeaderSize = kVersionBytes + kTraceIdBytes + kParentIdBytes + kTraceFlagBytes + kTraceDelimiterBytes;
        static const int kTraceStateMaxMembers = 32;
        static const nostd::string_view kTraceStateKeyValueDelimiter = "=";
        static const int kHeaderElementLengths[4] = {2,32,16,2};

        // TODO: need review on hex_string because trace ids are objects not string_views
        static void InjectImpl(Setter setter, T &carrier, const SpanContext &span_context) {
            char hex_string[kHeaderSize];
            sprintf(hex_string, "00-%032x-%016x-%02x",span_context.GetTraceId(),span_context.GetSpanId(),span_context.GetTraceFlags());
            nostd::string_view trace_parent = nostd::string_view(hex_string, kHeaderSize);
            setter(carrier, kTraceParent, trace_parent);
            if (span_context.GetTraceState() != NULL) {
                nostd::string_view trace_state = FormatTracestate(span_context.GetTraceState()); // I need the definition for the type of TraceState(Dictionary or something else). The trace state data structure will determine how I will able to join this together.
                setter(carrier, kTraceState, trace_state);
            }
        }

        static nostd::string_view FormatTracestate(TraceState trace_state) {
            nostd::string_view res = nostd::string_view("");
            KeyValueIterable entries = trace_state.GetEntries();
            entries.ForEachKeyValue([&res,&i](nostd::string_view key, nostd::string_view value) { // Is this usage correct?
                i++;
                res += key + "=" + value;
                if (i != entries.size()-1) res += ",";
            });
            return res;
        }

        static SpanContext ExtractContextFromTraceParent(nostd::string_view &trace_parent) {
            bool is_valid = trace_parent.length() == kHeaderSize
                            && trace_parent[kVersionBytes] == "-"
                            && trace_parent[kVersionBytes+kTraceIdBytes+1] == "-"
                            && trace_parent[kVersionBytes+kTraceIdBytes+kParentIdBytes+2] == "-";
            if (!is_valid) {
                std::cout<<"Unparseable trace_parent header. Returning INVALID span context."<<std::endl;
                return SpanContext.GetInvalid();
            }

            try {
                nostd::string_view version;
                nostd::string_view trace_id;
                nostd::string_view span_id;
                nostd::string_view trace_flags;
                int elt_num = 0;
                int countdown = kHeaderElementLengths[elt_num];
                int start_pos = -1;
                for (int i = 0; i < trace_parent.length(); i++) {
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
                    return trace.SetSpanInContext(trace::DefaultSpan.GetInvalid(), context);
                }
                if (version == "ff") {
                    return trace.SetSpanInContext(trace::DefaultSpan.GetInvalid(), context);
                }

                TraceId trace_id_obj = TraceId.FromLowerBase16(trace_id);
                SpanId span_id_obj = SpanId.FromLowerBase16(span_id);
                TraceFlags trace_flags_obj = TraceFlags.FromLowerBase16(trace_flags);
                return SpanContext(trace_id_obj, span_id_obj, true, trace_flags_obj, trace::TraceState.GetDefault());
            } catch (std::exception& e) {
                std::cout<<"Unparseable trace_parent header. Returning INVALID span context."<<std::endl;
                return SpanContext.GetInvalid();
            }
        }

        static void SetTraceStateBuilder(TraceState.Builder &trace_state_builder, nostd::string_view &list_member) {
            int index = -1;
            for (int j = 0; j < list_member.length(); j++) {
                if (list_member[j] == kTraceStateKeyValueDelimiter) {
                    index = j;
                    break;
                }
            }
            if (index == -1) {
                throw std::invalid_argument("Invalid TraceState list-member format.");
            }
            trace_state_builder.Set(list_member.substr(0, index), list_member.substr(index + 1));
        }

        static TraceState ExtractTraceState(nostd::string_view &trace_state_header) {
            TraceState.Builder trace_state_builder = TraceState.builder();
            int start_pos = -1;
            int end_pos = -1;
            int element_num = 0;
            nostd::string_view list_member;
            for (int i = 0; i < trace_state_header.length(); i++) {
                if (trace_state_header[i]=='\t') continue;
                else if (trace_state_header[i]==',') {
                    if (start_pos == -1 && end_pos == -1) continue;
                    element_num++;
                    list_member = trace_state_header.substr(start_pos,end_pos-start_pos+1);
                    SetTraceStateBuilder(trace_state_builder,list_member);
                    end_pos = -1;
                    start_pos = -1;
                } else {
                    end_pos = i;
                    if (start_pos==-1) start_pos = i;
                }
            }
            if (start_pos!=-1 && end_pos!=-1) {
                list_member = trace_state_header.substr(start_pos,end_pos-start_pos+1);
                SetTraceStateBuilder(trace_state_builder,list_member);
                element_num++;
            }

            if (element_num <= kTraceStateMaxMembers) {
                throw std::invalid_argument("TraceState has too many elements.");
            }
            return trace_state_builder.Build();
        }

        static SpanContext ExtractImpl(Getter getter, T &carrier) {
            nostd::string_view trace_parent = getter(carrier, kTraceParent);
            if (trace_parent == NULL) {
                return SpanContext.GetInvalid();
            }

            SpanContext context_from_parent_header = ExtractContextFromTraceParent(trace_parent);
            if (!context_from_parent_header.IsValid()) {
                return context_from_parent_header;
            }

            nostd::string_view trace_state_header = getter(carrier, kTraceState);
            if (trace_state_header == NULL || trace_state_header.isEmpty()) {
                return context_from_parent_header;
            }

            try {
                TraceState trace_state = ExtractTraceState(trace_state_header);
                // Need getter support from SpanContext
                return SpanContext(
                    context_from_parent_header.GetTraceId(),
                    context_from_parent_header.GetSpanId(),
                    true,
                    context_from_parent_header.GetTraceFlags(),
                    trace_state);
            } catch (std::exception& e) {
                std::cout<<"Unparseable tracestate header. Returning span context without state."<<std::endl;
                return SpanContext(
                    context_from_parent_header.GetTraceId(),
                    context_from_parent_header.GetSpanId(),
                    true,
                    context_from_parent_header.GetTraceFlags(),
                    TraceState.Builder().Build());
            }
        }
}
}
}
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
