/*
 * Copyright The OpenTelemetry Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <map>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "opentelemetry/sdk/common/attribute_utils.h"  // same as traces/attribute_utils
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
  // Define a JSON object that will be populated with the log data
  nlohmann::json json_;

  /**
   * A helper method that writes a key/value pair under a specified name, the two names used here
   * being "attributes" and "resources"
   */
  void WriteKeyValue(nostd::string_view key,
                     const opentelemetry::common::AttributeValue &value,
                     std::string name)
  {
    switch (value.index())
    {
      case common::AttributeType::TYPE_BOOL:
        json_[name][key.data()] = opentelemetry::nostd::get<bool>(value) ? true : false;
        return;
      case common::AttributeType::TYPE_INT:
        json_[name][key.data()] = opentelemetry::nostd::get<int>(value);
        return;
      case common::AttributeType::TYPE_INT64:
        json_[name][key.data()] = opentelemetry::nostd::get<int64_t>(value);
        return;
      case common::AttributeType::TYPE_UINT:
        json_[name][key.data()] = opentelemetry::nostd::get<unsigned int>(value);
        return;
      case common::AttributeType::TYPE_UINT64:
        json_[name][key.data()] = opentelemetry::nostd::get<uint64_t>(value);
        return;
      case common::AttributeType::TYPE_DOUBLE:
        json_[name][key.data()] = opentelemetry::nostd::get<double>(value);
        return;
      case common::AttributeType::TYPE_STRING:
#ifdef HAVE_CSTRING_TYPE
      case common::AttributeType::TYPE_CSTRING:
#endif
        json_[name][key.data()] =
            opentelemetry::nostd::get<opentelemetry::nostd::string_view>(value).data();
        return;
      default:
        return;
    }
  }

public:
  /**
   * Set the severity for this log.
   * @param severity the severity of the event
   */
  void SetSeverity(opentelemetry::logs::Severity severity) noexcept override
  {
    // Convert the severity enum to a string
    json_["severity"] = opentelemetry::logs::SeverityNumToText[static_cast<int>(severity)];
  }

  /**
   * Set name for this log
   * @param name the name to set
   */
  void SetName(nostd::string_view name) noexcept override { json_["name"] = name.data(); }

  /**
   * Set body field for this log.
   * @param message the body to set
   */
  void SetBody(nostd::string_view message) noexcept override { json_["body"] = message.data(); }

  /**
   * Set a resource for this log.
   * @param name the name of the resource
   * @param value the resource value
   */
  void SetResource(nostd::string_view key,
                   const opentelemetry::common::AttributeValue &value) noexcept override
  {
    WriteKeyValue(key, value, "resource");
  }

  /**
   * Set an attribute of a log.
   * @param key the key of the attribute
   * @param value the attribute value
   */
  void SetAttribute(nostd::string_view key,
                    const opentelemetry::common::AttributeValue &value) noexcept override
  {
    WriteKeyValue(key, value, "attributes");
  }

  /**
   * Set trace id for this log.
   * @param trace_id the trace id to set
   */
  void SetTraceId(opentelemetry::trace::TraceId trace_id) noexcept override
  {
    char trace_buf[32];
    trace_id.ToLowerBase16(trace_buf);
    json_["traceid"] = std::string(trace_buf, sizeof(trace_buf));
  }

  /**
   * Set span id for this log.
   * @param span_id the span id to set
   */
  virtual void SetSpanId(opentelemetry::trace::SpanId span_id) noexcept override
  {
    char span_buf[16];
    span_id.ToLowerBase16(span_buf);
    json_["spanid"] = std::string(span_buf, sizeof(span_buf));
  }

  /**
   * Inject a trace_flags  for this log.
   * @param trace_flags the span id to set
   */
  void SetTraceFlags(opentelemetry::trace::TraceFlags trace_flags) noexcept override
  {
    char flag_buf[2];
    trace_flags.ToLowerBase16(flag_buf);
    json_["traceflags"] = std::string(flag_buf, sizeof(flag_buf));
  }

  /**
   * Set the timestamp for this log.
   * @param timestamp the timestamp of the event
   */
  void SetTimestamp(core::SystemTimestamp timestamp) noexcept override
  {
    json_["timestamp"] = timestamp.time_since_epoch().count();
  }

  /**
   * Returns a JSON object contain the log information
   */
  nlohmann::json GetJSON() noexcept { return json_; };
};
}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
