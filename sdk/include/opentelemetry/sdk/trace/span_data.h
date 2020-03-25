#pragma once

#include <chrono>
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * SpanData is an immutable representation of all data collected by a span.
 */
class SpanData final : public Recordable
{
public:
  // The trace id for this span.
  opentelemetry::trace::TraceId GetTraceId() const noexcept = 0;

  // The span id for this span.
  opentelemetry::trace::SpanId GetSpanId() const noexcept = 0;

  // The span id for this span's parent.
  opentelemetry::trace::SpanId GetParentSpanId() const noexcept = 0;

  // The name of this span.
  opentelemetry::nostd::string_view GetName() const noexcept = 0;

  // The status of this span.
  opentelemetry::trace::CanonicalCode GetStatus() const noexcept = 0;

  // The start time of this span.
  opentelemetry::core::SystemTimestamp GetStartTime() const noexcept = 0;

  // The duration of this span.
  std::chrono::nanoseconds GetDuration() const noexcept = 0;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
