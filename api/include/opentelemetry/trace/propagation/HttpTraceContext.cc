#include <vector>

namespace trace
{
namespace propagation
{
namespace
{
class HttpTraceContext : public HTTPTextFormat
{

    private:
        static const nostd::string_view TRACE_PARENT = "traceparent";
        static const nostd::string_view TRACE_STATE = "tracestate";
        static const std::vector<nostd::string_view> FIELDS = {TRACE_PARENT, TRACE_STATE)};
        static const int VERSION_BYTES = 2;
        static const int TRACE_ID_BYTES = 32;
        static const int PARENT_ID_BYTES = 16;
        static const int TRACE_FLAG_BYTES = 2;
        static const int TRACE_DELIMITER_BYTES = 3;
        static const int HEADER_SIZE = VERSION_BYTES + TRACE_ID_BYTES + PARENT_ID_BYTES + TRACE_FLAG_BYTES + TRACE_DELIMITER_BYTES;

        static T checkNotNull(T arg, nostd::string_view errorMessage) {
            if (arg == NULL) {
                throw new NullPointerException(errorMessage);
            }
            return arg;
        }

        static void injectImpl(Setter setter, T &carrier, const SpanContext spanContext) {
            char hex_string[HEADER_SIZE];
            sprintf(hex_string, "00-%032x-%016x-%02x",span_context.trace_id,span_context.span_id,span_context.trace_flags);
            nostd::string_view traceparent_string(hex_string); // I don't know if the string view has a function that takes in an char array convert it to string though.
            setter(carrier, TRACE_PARENT, traceparent_string);
            if (spanContext.getTraceState()) {
                nostd::string_view tracestate_string = formatTracestate(span_context.getTraceState()); // I need the definition for the type of TraceState(Dictionary or something else)
                setter(carrier, TRACE_STATE, tracestate_string);
            }
        }

        static SpanContext extractContextFromTraceParent(nostd::string_view traceparent) {
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
                depending on what is the data structure of TraceState
            */
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
    public:
        List<String> fields() {
            return FIELDS;
        }

        void inject(Setter setter, T &carrier, const Context &context) {
            checkNotNull(context, "context");
            checkNotNull(setter, "setter");

            Span span = GetCurrentSpan(context);
            if (span == null || !span.getContext().isValid()) {
                // We don't have span.getContext() in span.h, should we just use span? As well as acquiring validity. (I do know how to implement them though)
                return;
            }
            injectImpl(span.getContext(), carrier, setter); // span.getContext() function needed
        }

        Context extract(Getter get_from_carrier, const T &carrier, Context &context) {
            checkNotNull(context, "context");
            checkNotNull(carrier, "carrier");
            checkNotNull(getter, "getter");

            SpanContext spanContext = extractImpl(carrier, getter);
            return SetSpanInContext(trace.DefaultSpan(spanContext), context); // I actually need a default span class (implemented) here, I don't know who to ask for though. But both python and java version have default span classes though.
        }
}
}
}
}  // namespace trace
