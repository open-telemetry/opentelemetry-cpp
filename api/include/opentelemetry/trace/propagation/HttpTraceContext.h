#include <vector>
#include <exception>
#include "opentelemetry/trace/propagation/httptextformat.h"
#include "opentelemetry/trace/spancontext.h"
#include "opentelemetry/trace/trace_state.h"
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

static Context SetSpanInContext(Span span, Context &context) {
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
    Span span = get_value(Context.SPAN_KEY, context);
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
        List<nostd::string_view> fields() {
            static const auto* FIELDS = new std::vector<nostd::string_view>({TRACE_PARENT, TRACE_STATE});
            return FIELDS;
        }

        void inject(Setter setter, T &carrier, const Context &context) override {
            common::AttributeValue span = GetCurrentSpan(context);
            if (span == NULL || !span.getContext().isValid()) {
                // We don't have span.getContext() in span.h, should we just use span? As well as acquiring validity. (I do know how to implement them though)
                return;
            }
            injectImpl(setter, carrier, span.getContext());
        }

        Context extract(Getter getter, const T &carrier, Context &context) override {
            SpanContext spanContext = extractImpl(carrier, getter);
            return SetSpanInContext(trace.DefaultSpan(spanContext), context);
        }

    private:
        static const nostd::string_view TRACE_PARENT = "traceparent";
        static const nostd::string_view TRACE_STATE = "tracestate";
        static const int VERSION_BYTES = 2;
        static const int TRACE_ID_BYTES = 32;
        static const int PARENT_ID_BYTES = 16;
        static const int TRACE_FLAG_BYTES = 2;
        static const int TRACE_DELIMITER_BYTES = 3;
        static const int HEADER_SIZE = VERSION_BYTES + TRACE_ID_BYTES + PARENT_ID_BYTES + TRACE_FLAG_BYTES + TRACE_DELIMITER_BYTES;
        static const int TRACESTATE_MAX_MEMBERS = 32;
        static const nostd::string_view TRACESTATE_KEY_VALUE_DELIMITER = "=";
        static const int HEADER_ELEMENT_LENGTHS[4] = {2,32,16,2};

        static void injectImpl(Setter setter, T &carrier, const SpanContext &spanContext) {
            char hex_string[HEADER_SIZE];
            sprintf(hex_string, "00-%032x-%016x-%02x",spanContext.trace_id(),spanContext.span_id(),spanContext.trace_flags());
            nostd::string_view traceparent_string = nostd::string_view(hex_string, HEADER_SIZE);
            setter(carrier, TRACE_PARENT, traceparent_string);
            if (spanContext.getTraceState()) {
                nostd::string_view tracestate_string = formatTracestate(spanContext.getTraceState()); // I need the definition for the type of TraceState(Dictionary or something else). The trace state data structure will determine how I will able to join this together.
                setter(carrier, TRACE_STATE, tracestate_string);
            }
        }

        static SpanContext extractContextFromTraceParent(nostd::string_view &traceparent) {
            bool isValid = traceparent.length() == HEADER_SIZE
                            && traceparent[VERSION_BYTES] == "-"
                            && traceparent[VERSION_BYTES+TRACE_ID_BYTES+1] == "-"
                            && traceparent[VERSION_BYTES+TRACE_ID_BYTES+PARENT_ID_BYTES+2] == "-";
            if (!isValid) {
                std::cout<<"Unparseable traceparent header. Returning INVALID span context."<<std::endl;
                return SpanContext.getInvalid();
            }

            try {
                nostd::string_view version;
                nostd::string_view traceid;
                nostd::string_view spanid;
                nostd::string_view traceflags;
                int eltNum = 0;
                int countdown = HEADER_ELEMENT_LENGTHS[eltNum];
                int startPos = -1;
                for (int i = 0; i < traceparent.length(); i++) {
                    if (traceparent[i]=='\t') continue;
                    else if (traceparent[i]=='-') {
                        if (countdown==0) {
                            if (eltNum == 0) {
                                version = traceparent.substr(startPos,HEADER_ELEMENT_LENGTHS[eltNum]);
                            } else if (eltNum == 1) {
                                traceid = traceparent.substr(startPos,HEADER_ELEMENT_LENGTHS[eltNum]);
                            } else if (eltNum == 2) {
                                spanid = traceparent.substr(startPos,HEADER_ELEMENT_LENGTHS[eltNum]);
                            } else {
                                throw; // Impossible to have more than 4 elements in parent header
                            }
                            countdown = HEADER_ELEMENT_LENGTHS[++eltNum];
                            startPos = -1;
                        } else {
                            throw;
                        }
                    } else if ((traceparent[i]>='a'&&traceparent[i]<='f')||(traceparent[i]>='0'&&traceparent[i]<='9')) {
                        if (startPos == -1) startPos = i;
                        countdown--;
                    } else {
                        throw;
                    }
                }
                traceflags = traceparent.substr(startPos,HEADER_ELEMENT_LENGTHS[eltNum]);

                if (trace_id == "00000000000000000000000000000000" || span_id == "0000000000000000") {
                    return trace.set_span_in_context(trace.INVALID_SPAN, context);
                }
                if (version == "ff") {
                    return trace.set_span_in_context(trace.INVALID_SPAN, context);
                }

                TraceId traceId = TraceId.fromLowerBase16(traceid);
                SpanId spanId = SpanId.fromLowerBase16(spanid);
                TraceFlags traceFlags = TraceFlags.fromLowerBase16(traceflags);
                return SpanContext.createFromRemoteParent(traceId, spanId, traceFlags, TRACE_STATE_DEFAULT);
            } catch (std::exception& e) {
                std::cout<<"Unparseable traceparent header. Returning INVALID span context."<<std::endl;
                return SpanContext.getInvalid();
            }
        }

        static void setTraceStateBuilder(TraceState.Builder &traceStateBuilder, nostd::string_view &listMember) {
            int index = -1;
            for (int j = 0; j < listMember.length(); j++) {
                if (listMember[j] == TRACESTATE_KEY_VALUE_DELIMITER) {
                    index = j;
                    break;
                }
            }
            if (index == -1) {
                throw std::invalid_argument("Invalid TraceState list-member format.");
            }
            traceStateBuilder.set(listMember.substring(0, index), listMember.substring(index + 1));
        }

        static TraceState extractTraceState(nostd::string_view &traceStateHeader) {
            TraceState.Builder traceStateBuilder = TraceState.builder();
            int startPos = -1;
            int endPos = -1;
            int elementNum = 0;
            nostd::string_view listMember;
            for (int i = 0; i < traceStateHeader.length(); i++) {
                if (traceStateHeader[i]=='\t') continue;
                else if (traceStateHeader[i]==',') {
                    if (startPos == -1 && endPos == -1) continue;
                    elementNum++;
                    listMember = traceStateHeader.substr(startPos,endPos-startPos+1);
                    setTraceStateBuilder(traceStateBuilder,listMember);
                    endPos = -1;
                    startPos = -1;
                } else {
                    endPos = i;
                    if (startPos==-1) startPos = i;
                }
            }
            if (startPos!=-1 && endPos!=-1) {
                listMember = traceStateHeader.substr(startPos,endPos-startPos+1);
                setTraceStateBuilder(traceStateBuilder,listMember);
                elementNum++;
            }

            if (elementNum <= TRACESTATE_MAX_MEMBERS) {
                throw std::invalid_argument("TraceState has too many elements.");
            }
            return traceStateBuilder.build();
        }

        static SpanContext extractImpl(Getter getter, T &carrier) {
            nostd::string_view traceParent = getter(carrier, TRACE_PARENT);
            if (traceParent == NULL) {
                return SpanContext.getInvalid();
            }

            SpanContext contextFromParentHeader = extractContextFromTraceParent(traceParent);
            if (!contextFromParentHeader.isValid()) {
                return contextFromParentHeader;
            }

            nostd::string_view traceStateHeader = getter(carrier, TRACE_STATE);
            if (traceStateHeader == NULL || traceStateHeader.isEmpty()) {
                return contextFromParentHeader;
            }

            try {
                TraceState traceState = extractTraceState(traceStateHeader);
                // Need getter support from SpanContext
                return SpanContext(
                    contextFromParentHeader.getTraceId(),
                    contextFromParentHeader.getSpanId(),
                    true,
                    contextFromParentHeader.getTraceFlags(),
                    traceState);
            } catch (std::exception& e) {
                std::cout<<"Unparseable tracestate header. Returning span context without state."<<std::endl;
                return SpanContext(
                    contextFromParentHeader.getTraceId(),
                    contextFromParentHeader.getSpanId(),
                    true,
                    contextFromParentHeader.getTraceFlags(),
                    TraceState.builder().build());
            }
        }
}
}
}
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
