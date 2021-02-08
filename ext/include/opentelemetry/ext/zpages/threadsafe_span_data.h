#pragma once

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace zpages
{

/**
 * This class is a threadsafe version of span data used for zpages in OT
 */
class ThreadsafeSpanData final : public opentelemetry::sdk::trace::Recordable
{
public:
  /**
   * Get the trace id for this span
   * @return the trace id for this span
   */
  opentelemetry::trace::TraceId GetTraceId() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return trace_id_;
  }

  /**
   * Get the span id for this span
   * @return the span id for this span
   */
  opentelemetry::trace::SpanId GetSpanId() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return span_id_;
  }

  /**
   * Get the resources for this span
   * @return the resources for this span
   */
  const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>
  GetResources()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return resources_;
  }

  /**
   * Get the parent span id for this span
   * @return the span id for this span's parent
   */
  opentelemetry::trace::SpanId GetParentSpanId() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return parent_span_id_;
  }

  /**
   * Get the name for this span
   * @return the name for this span
   */
  opentelemetry::nostd::string_view GetName() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return name_;
  }

  /**
   * Get the status for this span
   * @return the status for this span
   */
  opentelemetry::trace::StatusCode GetStatus() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_code_;
  }

  /**
   * Get the status description for this span
   * @return the description of the the status of this span
   */
  opentelemetry::nostd::string_view GetDescription() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return status_desc_;
  }

  /**
   * Get the start time for this span
   * @return the start time for this span
   */
  opentelemetry::core::SystemTimestamp GetStartTime() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return start_time_;
  }

  /**
   * Get the duration for this span
   * @return the duration for this span
   */
  std::chrono::nanoseconds GetDuration() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return duration_;
  }

  /**
   * Get the attributes for this span
   * @return the attributes for this span
   */
  const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue>
  GetAttributes() const noexcept
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return attributes_;
  }

  void SetIds(opentelemetry::trace::TraceId trace_id,
              opentelemetry::trace::SpanId span_id,
              opentelemetry::trace::SpanId parent_span_id) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    trace_id_       = trace_id;
    span_id_        = span_id;
    parent_span_id_ = parent_span_id;
  }

  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    attributes_[std::string(key)] = nostd::visit(converter_, value);
  }

  void SetStatus(opentelemetry::trace::StatusCode code,
                 nostd::string_view description) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    status_code_ = code;
    status_desc_ = std::string(description);
  }

  void SetName(nostd::string_view name) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    name_ = std::string(name);
  }

  void SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept override
  {
    span_kind_ = span_kind;
  }

  void SetResourceAttribute(
      std::string key,
      const opentelemetry::sdk::common::OwnedAttributeValue &value) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    attributes_.insert(
        std::pair<std::string, opentelemetry::sdk::common::OwnedAttributeValue>(key, value));
  }

  void SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    start_time_ = start_time;
  }

  void SetDuration(std::chrono::nanoseconds duration) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    duration_ = duration;
  }

  void AddLink(const opentelemetry::trace::SpanContext &span_context,
               const opentelemetry::common::KeyValueIterable &attributes =
                   opentelemetry::common::KeyValueIterableView<std::map<std::string, int>>(
                       {})) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    (void)span_context;
    (void)attributes;
  }

  void AddEvent(
      nostd::string_view name,
      core::SystemTimestamp timestamp = core::SystemTimestamp(std::chrono::system_clock::now()),
      const opentelemetry::common::KeyValueIterable &attributes =
          opentelemetry::common::KeyValueIterableView<std::map<std::string, int>>(
              {})) noexcept override
  {
    std::lock_guard<std::mutex> lock(mutex_);
    events_.push_back(
        opentelemetry::sdk::trace::SpanDataEvent(std::string(name), timestamp, attributes));
  }

  ThreadsafeSpanData() {}
  ThreadsafeSpanData(const ThreadsafeSpanData &threadsafe_span_data)
      : ThreadsafeSpanData(threadsafe_span_data,
                           std::lock_guard<std::mutex>(threadsafe_span_data.mutex_))
  {}

private:
  ThreadsafeSpanData(const ThreadsafeSpanData &threadsafe_span_data,
                     const std::lock_guard<std::mutex> &)
      : trace_id_(threadsafe_span_data.trace_id_),
        span_id_(threadsafe_span_data.span_id_),
        parent_span_id_(threadsafe_span_data.parent_span_id_),
        start_time_(threadsafe_span_data.start_time_),
        duration_(threadsafe_span_data.duration_),
        name_(threadsafe_span_data.name_),
        status_code_(threadsafe_span_data.status_code_),
        status_desc_(threadsafe_span_data.status_desc_),
        attributes_(threadsafe_span_data.attributes_),
        events_(threadsafe_span_data.events_),
        converter_(threadsafe_span_data.converter_)
  {}

  mutable std::mutex mutex_;
  opentelemetry::trace::TraceId trace_id_;
  opentelemetry::trace::SpanId span_id_;
  opentelemetry::trace::SpanId parent_span_id_;
  core::SystemTimestamp start_time_;
  std::chrono::nanoseconds duration_{0};
  std::string name_;
  opentelemetry::trace::SpanKind span_kind_;
  opentelemetry::trace::StatusCode status_code_{opentelemetry::trace::StatusCode::kUnset};
  std::string status_desc_;
  std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> attributes_;
  std::vector<opentelemetry::sdk::trace::SpanDataEvent> events_;
  opentelemetry::sdk::common::AttributeConverter converter_;
  std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> resources_;
};
}  // namespace zpages
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
