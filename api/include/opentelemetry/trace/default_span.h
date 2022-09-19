// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{

/**
 * DefaultSpan provides a non-operational Span that propagates
 * the tracer context by wrapping it inside the Span object.
 */

class DefaultSpan : public Span
{
public:
  // Returns an invalid span.
  static DefaultSpan GetInvalid() { return DefaultSpan(SpanContext::GetInvalid()); }

  trace::SpanContext GetContext() const noexcept override { return span_context_; }

  bool IsRecording() const noexcept override { return false; }

  void SetAttribute(nostd::string_view /* key */,
                    const common::AttributeValue & /* value */) noexcept override
  {}

  void AddEvent(nostd::string_view /* name */) noexcept override {}

  void AddEvent(nostd::string_view /* name */,
                common::SystemTimestamp /* timestamp */) noexcept override
  {}

  void AddEvent(nostd::string_view /* name */,
                const common::KeyValueIterable & /* attributes */) noexcept override
  {}

  void AddEvent(nostd::string_view /* name */,
                common::SystemTimestamp /* timestamp */,
                const common::KeyValueIterable & /* attributes */) noexcept override
  {}

  void SetStatus(StatusCode /* status */, nostd::string_view /* description */) noexcept override {}

  void UpdateName(nostd::string_view /* name */) noexcept override {}

  void End(const EndSpanOptions & /* options */) noexcept override {}

  nostd::string_view ToString() const noexcept { return "DefaultSpan"; }

  DefaultSpan(SpanContext span_context) noexcept : span_context_(span_context) {}

  // movable and copiable
  DefaultSpan(DefaultSpan &&spn) noexcept : span_context_(spn.GetContext()) {}
  DefaultSpan(const DefaultSpan &spn) noexcept : span_context_(spn.GetContext()) {}

private:
  SpanContext span_context_;
};

}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
