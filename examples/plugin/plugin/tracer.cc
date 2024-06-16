// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <iostream>
#include <memory>
#include <new>
#include <string>
#include <utility>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/context/context_value.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_metadata.h"
#include "tracer.h"

using namespace opentelemetry;

namespace
{

class Span final : public trace::Span
{
public:
  Span(std::shared_ptr<Tracer> &&tracer,
       nostd::string_view name,
       const common::KeyValueIterable & /*attributes*/,
       const trace::SpanContextKeyValueIterable & /*links*/,
       const trace::StartSpanOptions & /*options*/) noexcept
      : tracer_{std::move(tracer)}, name_{name}, span_context_{trace::SpanContext::GetInvalid()}
  {
    std::cout << "StartSpan: " << name << "\n";
  }

  ~Span() override { std::cout << "~Span\n"; }

  // opentelemetry::trace::Span
  void SetAttribute(nostd::string_view /*name*/,
                    const common::AttributeValue & /*value*/) noexcept override
  {
    std::cout << "SetAttribute()\n";
  }

  void AddEvent(nostd::string_view /*name*/) noexcept override {
    std::cout << "AddEvent1()\n";
  }

  void AddEvent(nostd::string_view /*name*/,
                common::SystemTimestamp /*timestamp*/) noexcept override
  {
    std::cout << "AddEvent2()\n";
  }

  void AddEvent(nostd::string_view /*name*/,
                common::SystemTimestamp /*timestamp*/,
                const common::KeyValueIterable & /*attributes*/) noexcept override
  {
    std::cout << "AddEvent3()\n";
  }

  void AddEvent(nostd::string_view /*name*/,
                const common::KeyValueIterable & /*attributes*/) noexcept override
  {
    std::cout << "AddEvent4()\n";
  }

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  void AddLink(const trace::SpanContext & /* target */,
               const common::KeyValueIterable & /* attrs */) noexcept override
  {
    std::cout << "AddLink()\n";
  }

  void AddLinks(const trace::SpanContextKeyValueIterable & /* links */) noexcept override {
    std::cout << "AddLinks()\n";
  }
#endif

  void SetStatus(trace::StatusCode /*code*/, nostd::string_view /*description*/) noexcept override
  {
    std::cout << "SetStatus()\n";
  }

  void UpdateName(nostd::string_view /*name*/) noexcept override {
    std::cout << "UpdateName()\n";
  }

  void End(const trace::EndSpanOptions & /*options*/) noexcept override {
    std::cout << "End()\n";
  }

  bool IsRecording() const noexcept override { 
    std::cout << "IsRecording()\n";
    return true; 
    }

  trace::SpanContext GetContext() const noexcept override {
    std::cout << "GetContext()\n";
     return span_context_; 
     }

private:
  std::shared_ptr<Tracer> tracer_;
  std::string name_;
  trace::SpanContext span_context_;
};
}  // namespace

Tracer::Tracer(nostd::string_view /*output*/) {
    std::cout << "Tracer...\n";
}

nostd::shared_ptr<trace::Span> Tracer::StartSpan(nostd::string_view name,
                                                 const common::KeyValueIterable &attributes,
                                                 const trace::SpanContextKeyValueIterable &links,
                                                 const trace::StartSpanOptions &options) noexcept
{
    std::cout << "StartSpan...\n";
  return nostd::shared_ptr<trace::Span>{
      new (std::nothrow) Span{this->shared_from_this(), name, attributes, links, options}};
}
