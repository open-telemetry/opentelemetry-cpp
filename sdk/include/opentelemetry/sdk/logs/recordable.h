// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/common/attribute_value.h"
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/logs/severity.h"
#  include "opentelemetry/sdk/common/empty_attributes.h"
#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"
#  include "opentelemetry/sdk/resource/resource.h"
#  include "opentelemetry/trace/span.h"
#  include "opentelemetry/trace/span_context.h"
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
class Recordable
{
public:
  virtual ~Recordable() = default;

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp to set
   */
  virtual void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept = 0;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  virtual void SetSeverity(opentelemetry::logs::Severity severity) noexcept = 0;

  /**
   * Set name for this log
   * @param name the name to set
   */
  virtual void SetName(nostd::string_view name) noexcept = 0;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  virtual void SetBody(nostd::string_view message) noexcept = 0;

  /**
   * Set Resource of this log
   * @param Resource the resource to set
   */
  virtual void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept = 0;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  virtual void SetAttribute(nostd::string_view key,
                            const opentelemetry::common::AttributeValue &value) noexcept = 0;

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  virtual void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept = 0;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(opentelemetry::trace::SpanId span_id) noexcept = 0;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  virtual void SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept = 0;

  /**
   * Set instrumentation_library for this log.
   * @param instrumentation_library the instrumentation library to set
   */
  virtual void SetInstrumentationLibrary(
      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
          &instrumentation_library) noexcept = 0;
};
}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
