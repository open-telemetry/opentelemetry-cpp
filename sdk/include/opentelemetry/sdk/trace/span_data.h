#pragma once

#include <chrono>
#include <unordered_map>
#include <vector>
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
 * A counterpart to AttributeValue that makes sure a value is owned. This
 * replaces all non-owning references with owned copies.
 */
using SpanDataAttributeValue = nostd::variant<bool,
                                              int64_t,
                                              uint64_t,
                                              double,
                                              std::string,
                                              std::vector<bool>,
                                              std::vector<int64_t>,
                                              std::vector<uint64_t>,
                                              std::vector<double>,
                                              std::vector<std::string>>;

/**
 * Creates an owned copy (SpanDataAttributeValue) of a non-owning AttributeValue.
 */
struct AttributeConverter
{
  SpanDataAttributeValue operator()(bool v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(int v)
  {
    return SpanDataAttributeValue(static_cast<int64_t>(v));
  }
  SpanDataAttributeValue operator()(int64_t v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(unsigned int v)
  {
    return SpanDataAttributeValue(static_cast<uint64_t>(v));
  }
  SpanDataAttributeValue operator()(uint64_t v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(double v) { return SpanDataAttributeValue(v); }
  SpanDataAttributeValue operator()(nostd::string_view v)
  {
    return SpanDataAttributeValue(std::string(v));
  }
  SpanDataAttributeValue operator()(nostd::span<const bool> v) { return convertSpan<bool>(v); }
  SpanDataAttributeValue operator()(nostd::span<const int64_t> v)
  {
    return convertSpan<int64_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const unsigned int> v)
  {
    return convertSpan<uint64_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const uint64_t> v)
  {
    return convertSpan<uint64_t>(v);
  }
  SpanDataAttributeValue operator()(nostd::span<const double> v) { return convertSpan<double>(v); }
  SpanDataAttributeValue operator()(nostd::span<const int> v) { return convertSpan<int64_t>(v); }
  SpanDataAttributeValue operator()(nostd::span<const nostd::string_view> v)
  {
    return convertSpan<std::string>(v);
  }

  template <typename T, typename U = T>
  SpanDataAttributeValue convertSpan(nostd::span<const U> vals)
  {
    std::vector<T> copy;
    for (auto &val : vals)
    {
      copy.push_back(T(val));
    }

    return SpanDataAttributeValue(std::move(copy));
  }
};

/**
 * Class for storing events in SpanData.
 */
class SpanDataEvent
{
public:
  SpanDataEvent(std::string name, core::SystemTimestamp timestamp)
      : name_(name), timestamp_(timestamp)
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

private:
  std::string name_;
  core::SystemTimestamp timestamp_;
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
  const std::unordered_map<std::string, SpanDataAttributeValue> &GetAttributes() const noexcept
  {
    return attributes_;
  }

  /**
   * Get the events associated with this span
   * @return the events associated with this span
   */
  const std::vector<SpanDataEvent> &GetEvents() const noexcept { return events_; }

  void SetIds(opentelemetry::trace::TraceId trace_id,
              opentelemetry::trace::SpanId span_id,
              opentelemetry::trace::SpanId parent_span_id) noexcept override
  {
    trace_id_       = trace_id;
    span_id_        = span_id;
    parent_span_id_ = parent_span_id;
  }

  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept override
  {
    attributes_[std::string(key)] = nostd::visit(converter_, value);
  }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace_api::KeyValueIterable &attributes) noexcept override
  {
    events_.push_back(SpanDataEvent(std::string(name), timestamp));
    // TODO: handle attributes
  }

  void AddLink(opentelemetry::trace::SpanContext span_context,
               const trace_api::KeyValueIterable &attributes) noexcept override
  {
    (void)span_context;
    (void)attributes;
  }

  void SetStatus(trace_api::CanonicalCode code, nostd::string_view description) noexcept override
  {
    status_code_ = code;
    status_desc_ = std::string(description);
  }

  void SetName(nostd::string_view name) noexcept override { name_ = std::string(name); }

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
  std::unordered_map<std::string, SpanDataAttributeValue> attributes_;
  std::vector<SpanDataEvent> events_;
  AttributeConverter converter_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
