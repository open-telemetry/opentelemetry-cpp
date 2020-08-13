#pragma once
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"

#define pass
OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class DefaultSpan : public Span
{
public:
  // Returns an invalid span.
  static DefaultSpan GetInvalid() { return DefaultSpan(SpanContext::GetInvalid()); }

  trace::SpanContext GetContext() const noexcept { return span_context_; }

  bool IsRecording() const noexcept { return false; }

  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept { pass; }

  void AddEvent(nostd::string_view name) noexcept { pass; }

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept { pass; }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const KeyValueIterable &attributes) noexcept
  {
    pass;
  }

  void AddEvent(nostd::string_view name, const KeyValueIterable &attributes) noexcept
  {
    this->AddEvent(name, std::chrono::system_clock::now(), attributes);
  }

  void SetStatus(CanonicalCode status, nostd::string_view description) noexcept { pass; }

  void UpdateName(nostd::string_view name) noexcept { pass; }

  void End(const EndSpanOptions &options = {}) noexcept { pass; }

  nostd::string_view ToString() { return "DefaultSpan"; }

  DefaultSpan() = default;

  DefaultSpan(SpanContext span_context) { this->span_context_ = span_context; }

  // movable and copiable
  DefaultSpan(DefaultSpan &&spn) : span_context_(spn.GetContext()) {}
  DefaultSpan(const DefaultSpan &spn) : span_context_(spn.GetContext()) {}

private:
  SpanContext span_context_;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
