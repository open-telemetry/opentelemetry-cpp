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

#include "opentelemetry/exporters/zipkin/recordable.h"

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

const int kAttributeValueSize = 14;

void Recordable::SetIds(trace::TraceId trace_id,
                        trace::SpanId span_id,
                        trace::SpanId parent_span_id) noexcept
{
  char trace_id_lower_base16[trace::TraceId::kSize * 2] = {0};
  trace_id.ToLowerBase16(trace_id_lower_base16);
  char span_id_lower_base16[trace::SpanId::kSize * 2] = {0};
  span_id.ToLowerBase16(span_id_lower_base16);
  char parent_span_id_lower_base16[trace::SpanId::kSize * 2] = {0};
  parent_span_id.ToLowerBase16(parent_span_id_lower_base16);
  span_["id"]       = std::string(span_id_lower_base16, 16);
  span_["parentId"] = std::string(parent_span_id_lower_base16, 16);
  span_["traceId"]  = std::string(trace_id_lower_base16, 32);
}

void PopulateAttribute(nlohmann::json &attribute,
                       nostd::string_view key,
                       const opentelemetry::common::AttributeValue &value)
{
  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes
  static_assert(
      nostd::variant_size<opentelemetry::common::AttributeValue>::value == kAttributeValueSize,
      "AttributeValue contains unknown type");

  if (nostd::holds_alternative<bool>(value))
  {
    attribute[key.data()] = nostd::get<bool>(value);
  }
  else if (nostd::holds_alternative<int>(value))
  {
    attribute[key.data()] = nostd::get<int>(value);
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    attribute[key.data()] = nostd::get<int64_t>(value);
  }
  else if (nostd::holds_alternative<unsigned int>(value))
  {
    attribute[key.data()] = nostd::get<unsigned int>(value);
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    attribute[key.data()] = nostd::get<uint64_t>(value);
  }
  else if (nostd::holds_alternative<double>(value))
  {
    attribute[key.data()] = nostd::get<double>(value);
  }
  else if (nostd::holds_alternative<nostd::string_view>(value))
  {
    attribute[key.data()] = nostd::string_view(nostd::get<nostd::string_view>(value).data(),
                                               nostd::get<nostd::string_view>(value).size());
  }
  else if (nostd::holds_alternative<nostd::span<const bool>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const bool>>(value))
    {
      attribute[key.data()].push_back(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const int>>(value))
    {
      attribute[key.data()].push_back(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const int64_t>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const int64_t>>(value))
    {
      attribute[key.data()].push_back(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const unsigned int>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const unsigned int>>(value))
    {
      attribute[key.data()].push_back(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const uint64_t>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const uint64_t>>(value))
    {
      attribute[key.data()].push_back(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const double>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const double>>(value))
    {
      attribute[key.data()].push_back(val);
    }
  }
  else if (nostd::holds_alternative<nostd::span<const nostd::string_view>>(value))
  {
    attribute[key.data()] = {};
    for (const auto &val : nostd::get<nostd::span<const nostd::string_view>>(value))
    {
      attribute[key.data()].push_back(std::string(val.data(), val.size()));
    }
  }
}

void Recordable::SetAttribute(nostd::string_view key,
                              const opentelemetry::common::AttributeValue &value) noexcept
{
  if (!span_.contains("tags"))
  {
    span_["tags"] = nlohmann::json::object();
  }
  PopulateAttribute(span_["tags"], key, value);
}

void Recordable::AddEvent(nostd::string_view name,
                          core::SystemTimestamp timestamp,
                          const common::KeyValueIterable &attributes) noexcept
{
  nlohmann::json attrs = nlohmann::json::object();  // empty object
  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    PopulateAttribute(attrs, key, value);
    return true;
  });

  nlohmann::json annotation = {{"value", nlohmann::json::object({{name.data(), attrs}}).dump()},
                               {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                                                 timestamp.time_since_epoch())
                                                 .count()}};

  if (!span_.contains("annotations"))
  {
    span_["annotations"] = nlohmann::json::array();
  }
  span_["annotations"].push_back(annotation);
}

void Recordable::AddLink(const opentelemetry::trace::SpanContext &span_context,
                         const common::KeyValueIterable &attributes) noexcept
{
  // TODO: Currently not supported by specs:
  // https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/trace/sdk_exporters/zipkin.md
}

void Recordable::SetStatus(trace::StatusCode code, nostd::string_view description) noexcept
{
  span_["tags"]["otel.status_code"] = code;
  if (description.size())
    span_["tags"]["otel.status_description"] = description;
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_["name"] = name.data();
}

void Recordable::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
  span_["timestamp"] = start_time.time_since_epoch().count();
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  span_["duration"] = duration.count();
}

void Recordable::SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept {}
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
