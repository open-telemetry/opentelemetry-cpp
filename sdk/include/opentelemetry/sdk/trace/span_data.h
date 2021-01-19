#pragma once

#include <chrono>
#include <unordered_map>
#include <vector>
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
/**
 * Class for storing events in SpanData.
 */
class SpanDataEvent
{
public:
  SpanDataEvent(std::string name,
                core::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes)
      : name_(name), timestamp_(timestamp), attribute_map_(attributes)
  {}

  /**
   * Get the name for this event
   * @return the name for this event
   */
  std::string GetName() const noexcept { return name_; }

  /**
   * Get the timestamp for this event
   * @return the timestamp for this event
   */
  core::SystemTimestamp GetTimestamp() const noexcept { return timestamp_; }

  /**
   * Get the attributes for this event
   * @return the attributes for this event
   */
  const std::unordered_map<std::string, common::OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return attribute_map_.GetAttributes();
  }

private:
  std::string name_;
  core::SystemTimestamp timestamp_;
  common::AttributeMap attribute_map_;
};

/**
 * Class for storing links in SpanData.
 * TODO: Add getters for trace_id, span_id and trace_state when these are supported by SpanContext
 */
class SpanDataLink
{
public:
  SpanDataLink(opentelemetry::trace::SpanContext span_context,
               const opentelemetry::common::KeyValueIterable &attributes)
      : span_context_(span_context), attribute_map_(attributes)
  {}

  /**
   * Get the attributes for this link
   * @return the attributes for this link
   */
  const std::unordered_map<std::string, common::OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return attribute_map_.GetAttributes();
  }

private:
  opentelemetry::trace::SpanContext span_context_;
  common::AttributeMap attribute_map_;
};

/**
 * SpanData is a representation of all data collected by a span.
 */
class SpanData final : public Recordable
{
public:
  /**
   * Get the trace id for this span
   * @return the trace id for this span
   */
  opentelemetry::trace::TraceId GetTraceId() const noexcept { return trace_id_; }

  /**
   * Get the span id for this span
   * @return the span id for this span
   */
  opentelemetry::trace::SpanId GetSpanId() const noexcept { return span_id_; }

  /**
   * Get the parent span id for this span
   * @return the span id for this span's parent
   */
  opentelemetry::trace::SpanId GetParentSpanId() const noexcept { return parent_span_id_; }

  /**
   * Get the name for this span
   * @return the name for this span
   */
  opentelemetry::nostd::string_view GetName() const noexcept { return name_; }

  /**
   * Get the kind of this span
   * @return the kind of this span
   */
  opentelemetry::trace::SpanKind GetSpanKind() const noexcept { return span_kind_; }

  /**
   * Get the status for this span
   * @return the status for this span
   */
  opentelemetry::trace::CanonicalCode GetStatus() const noexcept { return status_code_; }

  /**
   * Get the status description for this span
   * @return the description of the the status of this span
   */
  opentelemetry::nostd::string_view GetDescription() const noexcept { return status_desc_; }

  /**
   * Get the start time for this span
   * @return the start time for this span
   */
  opentelemetry::core::SystemTimestamp GetStartTime() const noexcept { return start_time_; }

  /**
   * Get the duration for this span
   * @return the duration for this span
   */
  std::chrono::nanoseconds GetDuration() const noexcept { return duration_; }

  /**
   * Get the attributes for this span
   * @return the attributes for this span
   */
  const std::unordered_map<std::string, common::OwnedAttributeValue> &GetAttributes() const noexcept
  {
    return attribute_map_.GetAttributes();
  }

  /**
   * Get the events associated with this span
   * @return the events associated with this span
   */
  const std::vector<SpanDataEvent> &GetEvents() const noexcept { return events_; }

  /**
   * Get the links associated with this span
   * @return the links associated with this span
   */
  const std::vector<SpanDataLink> &GetLinks() const noexcept { return links_; }

  void SetIds(opentelemetry::trace::TraceId trace_id,
              opentelemetry::trace::SpanId span_id,
              opentelemetry::trace::SpanId parent_span_id) noexcept override
  {
    trace_id_       = trace_id;
    span_id_        = span_id;
    parent_span_id_ = parent_span_id;
  }

  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override
  {
    attribute_map_.SetAttribute(key, value);
  }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    SpanDataEvent event(std::string(name), timestamp, attributes);
    events_.push_back(event);
  }

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const opentelemetry::common::KeyValueIterable &attributes) noexcept override
  {
    SpanDataLink link(span_context, attributes);
    links_.push_back(link);
  }

  void SetStatus(opentelemetry::trace::CanonicalCode code,
                 nostd::string_view description) noexcept override
  {
    status_code_ = code;
    status_desc_ = std::string(description);
  }

  void SetName(nostd::string_view name) noexcept override
  {
    name_ = std::string(name.data(), name.length());
  }

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override
  {
    span_kind_ = span_kind;
  }

  void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override
  {
    start_time_ = start_time;
  }

  void SetDuration(std::chrono::nanoseconds duration) noexcept override { duration_ = duration; }

private:
  opentelemetry::trace::TraceId trace_id_;
  opentelemetry::trace::SpanId span_id_;
  opentelemetry::trace::SpanId parent_span_id_;
  core::SystemTimestamp start_time_;
  std::chrono::nanoseconds duration_{0};
  std::string name_;
  opentelemetry::trace::CanonicalCode status_code_{opentelemetry::trace::CanonicalCode::OK};
  std::string status_desc_;
  common::AttributeMap attribute_map_;
  std::vector<SpanDataEvent> events_;
  std::vector<SpanDataLink> links_;
  opentelemetry::trace::SpanKind span_kind_{opentelemetry::trace::SpanKind::kInternal};
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
