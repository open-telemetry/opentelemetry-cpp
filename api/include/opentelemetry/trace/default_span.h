#pragma once
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/span.h"

#define pass
OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace {
class DefaultSpan: Span {
  public:
    // Returns an invalid span.
    static DefaultSpan GetInvalid() {
        return kInvalid;
    }

    // Creates an instance of this class with spancontext.
    static DefaultSpan Create(SpanContext span_context) {
      return DefaultSpan(span_context);
    }

    static DefaultSpan CreateRandom() {
      return DefaultSpan(
        SpanContext.Create(
          TraceId.generateRandomId(),
          SpanId.generateRandomId(),
          TraceFlags.getDefault(),
          TraceState.getDefault()
        )
      );
    }

    SpanContext GetContext() {
        return span_context_;
    }

    bool IsRecording() {
        return false;
    }
    
    void SetAttribute(nostd::string_view key, common::AttributeValue value) {
      pass;
    }

    void AddEvent(nostd::string_view name) { pass; }

    void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) { pass; }

    void AddEvent(nostd::string_view name,
                          core::SystemTimestamp timestamp,
                          const KeyValueIterable &attributes) { pass; }

    void AddEvent(nostd::string_view name, const KeyValueIterable &attributes)
    {
      this->AddEvent(name, std::chrono::system_clock::now(), attributes);
    }
    
    void SetStatus(CanonicalCode status, nostd::string_view description) {
      pass;
    }
  
    void UpdateName(nostd::string_view name) {
      pass;
    }

    void End(const EndSpanOptions &options = {}) {
      pass;
    }

    nostd::string_view ToString() {
      return "DefaultSpan";
    }

    DefaultSpan(SpanContext spanContext) {
       this.span_context_ = spanContext;
    }

    ~DefaultSpan() = default;

    Tracer &tracer() {
       return NULL; // Invalid tracer
    }

  private:
    static const DefaultSpan kInvalid = new DefaultSpan(SpanContext::GetInvalid());
    const SpanContext span_context_;
}
}