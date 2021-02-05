#include "tracer.h"
#include "opentelemetry/nostd/unique_ptr.h"

#include <iostream>
#include <memory>

namespace nostd   = opentelemetry::nostd;
namespace common  = opentelemetry::common;
namespace core    = opentelemetry::core;
namespace trace   = opentelemetry::trace;
namespace context = opentelemetry::context;

namespace
{
class Span final : public trace::Span
{
public:
  Span(std::shared_ptr<Tracer> &&tracer,
       nostd::string_view name,
       const opentelemetry::common::KeyValueIterable & /*attributes*/,
       const opentelemetry::trace::SpanContextKeyValueIterable & /*links*/,
       const trace::StartSpanOptions & /*options*/) noexcept
      : tracer_{std::move(tracer)}, name_{name}, span_context_{trace::SpanContext::GetInvalid()}
  {
    std::cout << "StartSpan: " << name << "\n";
  }

  ~Span() { std::cout << "~Span\n"; }

  // opentelemetry::trace::Span
  void SetAttribute(nostd::string_view /*name*/,
                    const common::AttributeValue & /*value*/) noexcept override
  {}

  void AddEvent(nostd::string_view /*name*/) noexcept override {}

  void AddEvent(nostd::string_view /*name*/, core::SystemTimestamp /*timestamp*/) noexcept override
  {}

  void AddEvent(nostd::string_view /*name*/,
                core::SystemTimestamp /*timestamp*/,
                const common::KeyValueIterable & /*attributes*/) noexcept override
  {}

  void SetStatus(trace::StatusCode /*code*/, nostd::string_view /*description*/) noexcept override
  {}

  void UpdateName(nostd::string_view /*name*/) noexcept override {}

  void End(const trace::EndSpanOptions & /*options*/) noexcept override {}

  bool IsRecording() const noexcept override { return true; }

  trace::SpanContext GetContext() const noexcept override { return span_context_; }

private:
  std::shared_ptr<Tracer> tracer_;
  std::string name_;
  trace::SpanContext span_context_;
};
}  // namespace

Tracer::Tracer(nostd::string_view /*output*/) {}

nostd::shared_ptr<trace::Span> Tracer::StartSpan(
    nostd::string_view name,
    const opentelemetry::common::KeyValueIterable &attributes,
    const opentelemetry::trace::SpanContextKeyValueIterable &links,
    const trace::StartSpanOptions &options) noexcept
{
  return nostd::shared_ptr<opentelemetry::trace::Span>{
      new (std::nothrow) Span{this->shared_from_this(), name, attributes, links, options}};
}
