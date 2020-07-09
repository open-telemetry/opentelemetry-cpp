#pragma once
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/spancontext.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/common/attribute_value.h"

#define pass
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

    SpanContext getContext() {
        return spanContext;
    }

    bool isRecordingEvents() {
        return false;
    }
    
    void setAttribute(nostd::string_view key, common::AttributeValue value) {
      pass;
    }

    void addEvent(nostd::string_view name, common::Attributes attributes, int timestamp) {
      pass;
    }
    
    void setStatus(CanonicalCode status) {
      pass;
    }
  
    void updateName(nostd::string_view name) {
      pass;
    }

    void end(int endTime) {
      pass;
    }

    nostd::string_view toString() {
      return "DefaultSpan";
    }

    DefaultSpan(SpanContext spanContext) {
       this.spanContext = spanContext;
    }

  private:
    static const DefaultSpan INVALID = new DefaultSpan(SpanContext.getInvalid());
    const SpanContext spanContext;
}
}