// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

/**
 * A non-recording span used when a span is not sampled.
 */
class NonRecordingSpan final : public opentelemetry::trace::Span
{
public:
  explicit NonRecordingSpan(opentelemetry::trace::SpanContext context) noexcept
      : context_(std::move(context))
  {}

  NonRecordingSpan(const NonRecordingSpan &)            = delete;
  NonRecordingSpan(NonRecordingSpan &&)                 = delete;
  NonRecordingSpan &operator=(const NonRecordingSpan &) = delete;
  NonRecordingSpan &operator=(NonRecordingSpan &&)      = delete;

  ~NonRecordingSpan() override = default;

  void SetAttribute(opentelemetry::nostd::string_view /*key*/,
                    const opentelemetry::common::AttributeValue & /*value*/) noexcept override
  {}

  void AddEvent(opentelemetry::nostd::string_view /*name*/) noexcept override {}

  void AddEvent(opentelemetry::nostd::string_view /*name*/,
                opentelemetry::common::SystemTimestamp /*timestamp*/) noexcept override
  {}

  void AddEvent(opentelemetry::nostd::string_view /*name*/,
                const opentelemetry::common::KeyValueIterable & /*attributes*/) noexcept override
  {}

  void AddEvent(opentelemetry::nostd::string_view /*name*/,
                opentelemetry::common::SystemTimestamp /*timestamp*/,
                const opentelemetry::common::KeyValueIterable & /*attributes*/) noexcept override
  {}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  void AddLink(const opentelemetry::trace::SpanContext & /*target*/,
               const opentelemetry::common::KeyValueIterable & /*attrs*/) noexcept override
  {}

  void AddLinks(
      const opentelemetry::trace::SpanContextKeyValueIterable & /*links*/) noexcept override
  {}
#endif

  void SetStatus(opentelemetry::trace::StatusCode /*code*/,
                 opentelemetry::nostd::string_view /*description*/) noexcept override
  {}

  void UpdateName(opentelemetry::nostd::string_view /*name*/) noexcept override {}

  void End(const opentelemetry::trace::EndSpanOptions & /*options*/ = {}) noexcept override {}

  bool IsRecording() const noexcept override { return false; }

  opentelemetry::trace::SpanContext GetContext() const noexcept override { return context_; }

private:
  opentelemetry::trace::SpanContext context_;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
