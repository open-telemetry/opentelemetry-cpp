// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <map>
#include <sstream>
#include <type_traits>
#include <unordered_map>

#include "nlohmann/json.hpp"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/logs/recordable.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{

/**
 * An Elasticsearch Recordable implemenation that stores the 10 fields of the Log Data Model inside
 * a JSON object,
 */
class ElasticSearchRecordable final : public sdk::logs::Recordable
{
private:
  /**
   * A helper method that writes a key/value pair under a specified name, the two names used here
   * being "attributes" and "resources"
   */
  void WriteKeyValue(nostd::string_view key,
                     const opentelemetry::common::AttributeValue &value,
                     std::string name);

  void WriteKeyValue(nostd::string_view key,
                     const opentelemetry::sdk::common::OwnedAttributeValue &value,
                     std::string name);

  void WriteValue(const opentelemetry::common::AttributeValue &value, std::string name);

public:
  /**
   * Returns a JSON object contain the log information
   */
  nlohmann::json GetJSON() noexcept;

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp to set
   */
  void SetTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /**
   * Set the observed timestamp for this log.
   * @param timestamp the timestamp to set
   */
  void SetObservedTimestamp(opentelemetry::common::SystemTimestamp timestamp) noexcept override;

  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  void SetSeverity(opentelemetry::logs::Severity severity) noexcept override;

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  void SetBody(const opentelemetry::common::AttributeValue &message) noexcept override;

  /**
   * Set the Event Id
   * @param id the event id to set
   * @param name the event name to set
   */
  void SetEventId(int64_t /* id */, nostd::string_view /* name */) noexcept override
  {
    // TODO: implement event id
  }

  /**
   * Set the trace id for this log.
   * @param trace_id the trace id to set
   */
  void SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept override;

  /**
   * Set the span id for this log.
   * @param span_id the span id to set
   */
  void SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept override;

  /**
   * Inject trace_flags for this log.
   * @param trace_flags the trace flags to set
   */
  void SetTraceFlags(const opentelemetry::trace::TraceFlags &trace_flags) noexcept override;

  /**
   * Set an attribute of a log.
   * @param key the name of the attribute
   * @param value the attribute value
   */
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override;

  /**
   * Set Resource of this log
   * @param Resource the resource to set
   */
  void SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept override;

  /**
   * Set instrumentation_scope for this log.
   * @param instrumentation_scope the instrumentation scope to set
   */
  void SetInstrumentationScope(const opentelemetry::sdk::instrumentationscope::InstrumentationScope
                                   &instrumentation_scope) noexcept override;

private:
  // Define a JSON object that will be populated with the log data
  nlohmann::json json_;
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
