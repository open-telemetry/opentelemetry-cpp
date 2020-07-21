#include "tracer.h"

#include <iostream>

namespace nostd  = opentelemetry::nostd;
namespace common = opentelemetry::common;
namespace core   = opentelemetry::core;
namespace trace  = opentelemetry::trace;

namespace
{
class Span final : public trace::Span
{
public:
  Span(std::shared_ptr<Tracer> &&tracer,
       nostd::string_view name,
       const opentelemetry::trace::KeyValueIterable & /*attributes*/,
       const trace::StartSpanOptions & /*options*/) noexcept
      : tracer_{std::move(tracer)}, name_{name}
  {
    std::cout << "StartSpan: " << name << "\n";
  }

  ~Span() { std::cout << "~Span\n"; }

  // opentelemetry::trace::Span
  void SetAttribute(nostd::string_view /*name*/,
                    const common::AttributeValue && /*value*/) noexcept override
  {}

  void AddEvent(nostd::string_view /*name*/) noexcept override {}

  void AddEvent(nostd::string_view /*name*/, core::SystemTimestamp /*timestamp*/) noexcept override
  {}

  void AddEvent(nostd::string_view /*name*/,
                core::SystemTimestamp /*timestamp*/,
                const trace::KeyValueIterable & /*attributes*/) noexcept override
  {}

  void SetStatus(trace::CanonicalCode /*code*/,
                 nostd::string_view /*description*/) noexcept override
  {}

  void UpdateName(nostd::string_view /*name*/) noexcept override {}

  void End(const trace::EndSpanOptions & /*options*/) noexcept override {}

  bool IsRecording() const noexcept override { return true; }

  Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<Tracer> tracer_;
  std::string name_;
};
}  // namespace

Tracer::Tracer(nostd::string_view /*output*/) {}

nostd::unique_ptr<trace::Span> Tracer::StartSpan(
    nostd::string_view name,
    const opentelemetry::trace::KeyValueIterable &attributes,
    const trace::StartSpanOptions &options) noexcept
{
  return nostd::unique_ptr<opentelemetry::trace::Span>{
      new (std::nothrow) Span{this->shared_from_this(), name, attributes, options}};
}
