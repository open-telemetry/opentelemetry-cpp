#pragma once
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/span.h"

#define pass
OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace {
class DefaultSpan: public Span {
  public:
    // Returns an invalid span.
    static DefaultSpan GetInvalid() {
        DefaultSpan invalid = DefaultSpan(SpanContext::GetInvalid());
        return invalid;
    }

    trace::SpanContext GetContext() const noexcept {
        return span_context_;
    }

    bool IsRecording() const noexcept {
        return false;
    }

    void SetAttribute(nostd::string_view key, const common::AttributeValue &&value) noexcept {
      pass;
    }

    void AddEvent(nostd::string_view name) noexcept { pass; }

    void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept { pass; }

    void AddEvent(nostd::string_view name,
                          core::SystemTimestamp timestamp,
                          const KeyValueIterable &attributes) noexcept { pass; }

    void AddEvent(nostd::string_view name, const KeyValueIterable &attributes) noexcept
    {
      this->AddEvent(name, std::chrono::system_clock::now(), attributes);
    }
    
    void SetStatus(CanonicalCode status, nostd::string_view description) noexcept {
      pass;
    }
  
    void UpdateName(nostd::string_view name) noexcept {
      pass;
    }

    void End(const EndSpanOptions &options = {}) noexcept {
      pass;
    }

    nostd::string_view ToString() {
      return "DefaultSpan";
    }

//    DefaultSpan(SpanContext span_context) {
//       this->span_context_ = span_context;
//    }

    DefaultSpan(SpanContext&& span_context) {
       this->span_context_ = span_context;
    }

//    Tracer &tracer() const noexcept {
//       return Tracer(); // Invalid tracer
//    }

    // Creates an instance of this class with spancontext.
    static DefaultSpan Create(SpanContext span_context) {
      return DefaultSpan(span_context);
    }

    static DefaultSpan CreateRandom() {
      return DefaultSpan(
//        SpanContext::Create(
//          TraceId::generateRandomId(),
//          SpanId::generateRandomId(),
//          TraceFlags::getDefault(),
//          TraceState::getDefault()
//        )
      );
    }

  private:
    SpanContext span_context_;
};
}
OPENTELEMETRY_END_NAMESPACE