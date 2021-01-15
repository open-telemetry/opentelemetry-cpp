#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "opentelemetry/version.h"

#include "opentelemetry/common/key_value_iterable_view.h"

#include <opentelemetry/nostd/span.h>
#include <map>
#include <string>
#include <vector>

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer_provider.h"

#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/span_data.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "opentelemetry/exporters/etw/etw_fields.h"
#include "opentelemetry/exporters/etw/etw_provider.h"
#include "opentelemetry/exporters/etw/utils.h"

#include "opentelemetry/exporters/etw/etw_properties.h"

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace ETW
{

class ETWSpanData final : public sdk::trace::Recordable
{
public:
  ETWSpanData(std::string providerName) { InitTracerProvider(providerName); }

  /**
   * Get the trace id for this span
   * @return the trace id for this span
   */
  trace::TraceId GetTraceId() const noexcept { return trace_id_; }

  /**
   * Get the span id for this span
   * @return the span id for this span
   */
  trace::SpanId GetSpanId() const noexcept { return span_id_; }

  /**
   * Get the parent span id for this span
   * @return the span id for this span's parent
   */
  trace::SpanId GetParentSpanId() const noexcept { return parent_span_id_; }

  /**
   * Get the name for this span
   * @return the name for this span
   */
  nostd::string_view GetName() const noexcept { return name_; }

  /**
   * Get the status for this span
   * @return the status for this span
   */
  trace::CanonicalCode GetStatus() const noexcept { return status_code_; }

  /**
   * Get the status description for this span
   * @return the description of the the status of this span
   */
  nostd::string_view GetDescription() const noexcept { return status_desc_; }

  /**
   * Get the start time for this span
   * @return the start time for this span
   */
  core::SystemTimestamp GetStartTime() const noexcept { return start_time_; }

  /**
   * Get the duration for this span
   * @return the duration for this span
   */
  std::chrono::nanoseconds GetDuration() const noexcept { return duration_; }

  /**
   * Get the attributes for this span
   * @return the attributes for this span
   */
  const std::unordered_map<std::string, sdk::common::OwnedAttributeValue> &GetAttributes() const
      noexcept
  {
    return attribute_map_.GetAttributes();
  }

  void SetIds(trace::TraceId trace_id,
              trace::SpanId span_id,
              trace::SpanId parent_span_id) noexcept override
  {
    trace_id_       = trace_id;
    span_id_        = span_id;
    parent_span_id_ = parent_span_id;
  }

  void SetAttribute(nostd::string_view key, const common::AttributeValue &value) noexcept override
  {
    attribute_map_.SetAttribute(key, value);
  }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const common::KeyValueIterable &attributes) noexcept override
  {
    span_->AddEvent(name, timestamp, attributes);
  }

  void AddLink(const trace::SpanContext &span_context,
               const common::KeyValueIterable &attributes) noexcept override
  {
    // TODO: Link Implementation for the Span to be implemented
  }

  /**
   * @brief Set recordable status code and description.
   * @param code
   * @param description
   * @return
   */
  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept override
  {
    status_code_ = code;
    status_desc_ = std::string(description);
  }

  /**
   * @brief Set recordable name.
   * @param name
   * @return
   */
  void SetName(nostd::string_view name) noexcept override { name_ = std::string(name); }

  /**
   * @brief Set span kind
   * @param span_kind
   * @return
   */
  void SetSpanKind(trace::SpanKind span_kind) noexcept override { span_kind_ = span_kind; }

  /**
   * @brief Set recordable start time.
   * @param start_time
   * @return
   */
  void SetStartTime(core::SystemTimestamp start_time) noexcept override
  {
    start_time_ = start_time;
  }

  /**
   * @brief Set recordable duration.
   * @param duration
   * @return
   */
  void SetDuration(std::chrono::nanoseconds duration) noexcept override { duration_ = duration; }

  /**
   * @brief Initialize ETW provider by name or GUID.
   * @param providerName - Provider Name or GUID that starts with `{`
   */
  void InitTracerProvider(std::string providerName)
  {
    TracerProvider tracer_provider_;

    tracer_ = tracer_provider_.GetTracer(providerName);
    span_   = tracer_->StartSpan(name_);
  }

private:
  trace::TraceId trace_id_;
  trace::SpanId span_id_;
  trace::SpanId parent_span_id_;
  core::SystemTimestamp start_time_;
  std::chrono::nanoseconds duration_{0};
  std::string name_;
  trace::CanonicalCode status_code_{trace::CanonicalCode::OK};
  std::string status_desc_;
  sdk::common::AttributeMap attribute_map_;
  trace::SpanKind span_kind_{trace::SpanKind::kInternal};
  nostd::shared_ptr<trace::Tracer> tracer_;
  nostd::shared_ptr<trace::Span> span_;
};

}  // namespace ETW
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
