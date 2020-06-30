#include <vector>
#include <regex>
#include <stdexcept>
#include "opentelemetry/trace/propagation/httptextformat.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/common/variant.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{
namespace
{

static Context SetSpanInContext(common::AttributeValue span, Context &context) {
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

static common::AttributeValue GetCurrentSpan(Context &context) {
    common::AttributeValue span = get_value(Context.SPAN_KEY, context);
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
            injectImpl(span.getContext(), carrier, setter); // span.getContext() function needed
        }

        Context extract(Getter getter, const T &carrier, Context &context) override {
            SpanContext spanContext = extractImpl(carrier, getter);
            return SetSpanInContext(trace.DefaultSpan(spanContext), context); // I actually need a default span class (implemented) here, I don't know who to ask for though. But both python and java version have default span classes though.
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
        static const std::regex TRACESTATE_ENTRY_DELIMITER_SPLIT_PATTERN("[ \t]*,[ \t]*");

        static Context checkNotNull(Context &arg, nostd::string_view errorMessage) {
            if (arg == NULL) {
                throw new NullPointerException("error: null " + errorMessage);
            }
            return arg;
        }

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
                // I am assuming that these functions are provided in those respective classes for this is the case in the Java version, despite the python version is using regular expression within this class.
                TraceId traceId = TraceId.fromLowerBase16(traceparent, TRACE_ID_OFFSET);
                SpanId spanId = SpanId.fromLowerBase16(traceparent, SPAN_ID_OFFSET);
                TraceFlags traceFlags = TraceFlags.fromLowerBase16(traceparent, TRACE_OPTION_OFFSET);
                return SpanContext.createFromRemoteParent(traceId, spanId, traceFlags, TRACE_STATE_DEFAULT);
            } catch (IllegalArgumentException e) {
                std::cout<<"Unparseable traceparent header. Returning INVALID span context."<<std::endl;
                return SpanContext.getInvalid();
            }
        }

        static TraceState extractTraceState(nostd::string_view traceStateHeader) {
            // TODO: implementation
            /** The question here is that how should I treat the trace state. Implementation will differ pretty much
                depending on what is the data structure of TraceState. Also this is based on the premise that
                trace state exists and has a builder
            */
            std::smatch listMembers;
            TraceState.Builder traceStateBuilder = TraceState.builder();
            regex_search(traceStateHeader,sm,TRACESTATE_ENTRY_DELIMITER_SPLIT_PATTERN); // I hope regex accepts string view
            if (listMembers.size() <= TRACESTATE_MAX_MEMBERS) {
                throw std::invalid_argument("TraceState has too many elements.");
            }
            for (int i = listMembers.size() - 1; i >= 0; i--) {
                nostd::string_view listMember = listMembers[i];
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
            return traceStateBuilder.build();
        }

        static SpanContext extractImpl(Getter getter, T &carrier) {
            nostd::string_view traceParent = getter(carrier, TRACE_PARENT);
            if (traceParent == NULL) {
                return SpanContext.getInvalid(); // I need support of this method from SpanContext definition which is currently unavailable it seems
            }

            SpanContext contextFromParentHeader = extractContextFromTraceParent(traceParent);
            if (!contextFromParentHeader.isValid()) {
                // Again, I still need the support of this method from context.
                return contextFromParentHeader;
            }

            nostd::string_view traceStateHeader = getter(carrier, TRACE_STATE);
            if (traceStateHeader == NULL || traceStateHeader.isEmpty()) {
                return contextFromParentHeader;
            }

            try {
                // Again, the trace state problem, should I treat it like a string or dictionary or an encapsuled class? This is not defined in current files
                TraceState traceState = extractTraceState(traceStateHeader);
                // Need getter support from SpanContext
                return SpanContext.createFromRemoteParent(
                    contextFromParentHeader.getTraceId(),
                    contextFromParentHeader.getSpanId(),
                    contextFromParentHeader.getTraceFlags(),
                    traceState);
            } catch (IllegalArgumentException e) {
                std::cout<<"Unparseable tracestate header. Returning span context without state."<<std::endl;
                return contextFromParentHeader;
            }
        }
}
}
}
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
