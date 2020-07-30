#pragma once
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/span.h"

#define pass
OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace {
class DefaultTracer: public Tracer {
public:
  ~DefaultTracer() = default;

  /**
   * Starts a span.
   *
   * Optionally sets attributes at Span creation from the given key/value pairs.
   *
   * Attributes will be processed in order, previous attributes with the same
   * key will be overwritten.
   */
  nostd::unique_ptr<Span> StartSpan(nostd::string_view name,
                                    const KeyValueIterable &attributes,
                                    const StartSpanOptions &options = {}) noexcept
  {
     return DefaultSpan::GetInvalid();
  }

  void ForceFlushWithMicroseconds(uint64_t timeout) noexcept
  {
    pass;
  }

  void CloseWithMicroseconds(uint64_t timeout) noexcept
  {
    pass;
  }
};
class DefaultSpan: public Span {
  public:
    // Returns an invalid span.
    static DefaultSpan GetInvalid() {
        return DefaultSpan(SpanContext::GetInvalid());
    }

    trace::SpanContext GetContext() const noexcept {
        return span_context_;
    }

    bool IsRecording() const noexcept {
        return false;
    }

    void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept {
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

    DefaultSpan() = default;

    DefaultSpan(SpanContext span_context) {
       this->span_context_ = span_context;
    }

    // movable and copiable
    DefaultSpan(DefaultSpan&& spn) : span_context_(spn.GetContext()) {}
    DefaultSpan(const DefaultSpan& spn) : span_context_(spn.GetContext()) {}

    trace::Tracer &tracer() const noexcept {
      return DefaultTracer();
    }

  private:
    SpanContext span_context_;
};
}
OPENTELEMETRY_END_NAMESPACE