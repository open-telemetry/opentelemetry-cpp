#include "opentelemetry/trace/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace {
public const class DefaultSpan : Span {
  public:
    // Returns an invalid span.
    static DefaultSpan getInvalid() {
        return INVALID;
    }

    // Creates an instance of this class with spancontext.
    static DefaultSpan create(SpanContext spanContext) {
      return DefaultSpan(spanContext);
    }

    static DefaultSpan createRandom() {
      return DefaultSpan(
        SpanContext(
          TraceId.generateRandomId(),
          SpanId.generateRandomId(),
          false,
          TraceFlags.getDefault(),
          TraceState.getDefault()
        )
      );
    }

    DefaultSpan(SpanContext spanContext) {
       this.spanContext = spanContext;
    }

  private:
    static const DefaultSpan INVALID = new DefaultSpan(SpanContext.getInvalid());
    const SpanContext spanContext;
}
}