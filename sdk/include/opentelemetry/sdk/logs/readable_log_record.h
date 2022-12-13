// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include <string>
#  include <unordered_map>

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/logs/severity.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/sdk/common/empty_attributes.h"
#  include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#  include "opentelemetry/sdk/logs/recordable.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/trace/span_id.h"
#  include "opentelemetry/trace/trace_flags.h"
#  include "opentelemetry/trace/trace_id.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
/**
 * Maintains a representation of a log in a format that can be processed by a recorder.
 *
 * This class is thread-compatible.
 */
class ReadableLogRecord : public Recordable
{
public:
  /**
   * Get the timestamp of this log.
   * @return the timestamp of this log
   */
  virtual opentelemetry::common::SystemTimestamp GetTimestamp() const noexcept = 0;

  /**
   * Get the observed timestamp of this log.
   * @return the observed timestamp of this log
   */
  virtual opentelemetry::common::SystemTimestamp GetObservedTimestamp() const noexcept = 0;

  /**
   * Get the severity of this log.
   * @return the severity of this log
   */
  virtual opentelemetry::logs::Severity GetSeverity() const noexcept = 0;

  /**
   * Get the severity text of this log.
   * @return the severity text for this log
   */
  virtual nostd::string_view GetSeverityText() const noexcept;

  /**
   * Get body field of this log.
   * @return the body field for this log.
   */
  virtual const opentelemetry::common::AttributeValue &GetBody() const noexcept = 0;

  /**
   * Get the trace id of this log.
   * @return the trace id of this log
   */
  virtual const opentelemetry::trace::TraceId &GetTraceId() const noexcept = 0;

  /**
   * Get the span id of this log.
   * @return the span id of this log
   */
  virtual const opentelemetry::trace::SpanId &GetSpanId() const noexcept = 0;

  /**
   * Inject trace_flags of this log.
   * @return trace_flags of this log
   */
  virtual const opentelemetry::trace::TraceFlags &GetTraceFlags() const noexcept = 0;

  /**
   * Get attributes of this log.
   * @return the body field of this log
   */
  virtual const std::unordered_map<std::string, opentelemetry::common::AttributeValue>
      &GetAttributes() const noexcept = 0;

  /**
   * Get resource of this log
   * @return the resource of this log
   */
  virtual const opentelemetry::sdk::resource::Resource &GetResource() const noexcept = 0;

  /**
   * Get instrumentation scope of this log.
   * @return  the instrumentation scope of this log
   */
  virtual const opentelemetry::sdk::instrumentationscope::InstrumentationScope &
  GetInstrumentationScope() const noexcept = 0;

  /**
   * Get default instrumentation scope of logs.
   * @return  the default instrumentation scope of logs
   */
  static const opentelemetry::sdk::instrumentationscope::InstrumentationScope &
  GetDefaultInstrumentationScope() noexcept;

  /**
   * Get default resource of logs.
   * @return  the default resource of logs
   */
  static const opentelemetry::sdk::resource::Resource &GetDefaultResource() noexcept;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
