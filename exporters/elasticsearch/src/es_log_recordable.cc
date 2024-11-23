// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
void ElasticSearchRecordable::WriteValue(
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    const std::string &name)
{
  namespace common = opentelemetry::sdk::common;
  switch (value.index())
  {
    case common::kTypeBool:
      json_[name] = opentelemetry::nostd::get<bool>(value) ? true : false;
      return;
    case common::kTypeInt:
      json_[name] = opentelemetry::nostd::get<int>(value);
      return;
    case common::kTypeInt64:
      json_[name] = opentelemetry::nostd::get<int64_t>(value);
      return;
    case common::kTypeUInt:
      json_[name] = opentelemetry::nostd::get<unsigned int>(value);
      return;
    case common::kTypeUInt64:
      json_[name] = opentelemetry::nostd::get<uint64_t>(value);
      return;
    case common::kTypeDouble:
      json_[name] = opentelemetry::nostd::get<double>(value);
      return;
    case common::kTypeString:
      json_[name] = opentelemetry::nostd::get<std::string>(value).data();
      return;
    default:
      return;
  }
}

void ElasticSearchRecordable::WriteValue(const opentelemetry::common::AttributeValue &value,
                                         const std::string &name)
{

  // Assert size of variant to ensure that this method gets updated if the variant
  // definition changes

  if (nostd::holds_alternative<bool>(value))
  {
    json_[name] = opentelemetry::nostd::get<bool>(value) ? true : false;
  }
  else if (nostd::holds_alternative<int>(value))
  {
    json_[name] = opentelemetry::nostd::get<int>(value);
  }
  else if (nostd::holds_alternative<int64_t>(value))
  {
    json_[name] = opentelemetry::nostd::get<int64_t>(value);
  }
  else if (nostd::holds_alternative<unsigned int>(value))
  {
    json_[name] = opentelemetry::nostd::get<unsigned int>(value);
  }
  else if (nostd::holds_alternative<uint64_t>(value))
  {
    json_[name] = opentelemetry::nostd::get<uint64_t>(value);
  }
  else if (nostd::holds_alternative<double>(value))
  {
    json_[name] = opentelemetry::nostd::get<double>(value);
  }
  else if (nostd::holds_alternative<const char *>(value))
  {
    json_[name] = std::string(nostd::get<const char *>(value));
  }
  else if (nostd::holds_alternative<nostd::string_view>(value))
  {
    json_[name] = static_cast<std::string>(opentelemetry::nostd::get<nostd::string_view>(value));
  }
  else if (nostd::holds_alternative<nostd::span<const uint8_t>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const uint8_t>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const bool>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const bool>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const int>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const int>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const int64_t>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const int64_t>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const unsigned int>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const unsigned int>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const uint64_t>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const uint64_t>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const double>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const double>>(value))
    {
      array_value.push_back(val);
    }
    json_[name] = array_value;
  }
  else if (nostd::holds_alternative<nostd::span<const nostd::string_view>>(value))
  {
    nlohmann::json array_value = nlohmann::json::array();
    for (const auto &val : nostd::get<nostd::span<const nostd::string_view>>(value))
    {
      array_value.push_back(static_cast<std::string>(val));
    }
    json_[name] = array_value;
  }
}

ElasticSearchRecordable::ElasticSearchRecordable() noexcept : sdk::logs::Recordable()
{
  json_["ecs"]["version"] = "8.11.0";
}

nlohmann::json ElasticSearchRecordable::GetJSON() noexcept
{
  return json_;
}

void ElasticSearchRecordable::SetTimestamp(
    opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  const std::chrono::system_clock::time_point timePoint{timestamp};

  // If built with with at least cpp 20 then use std::format
  // Otherwise use the old style to format the timestamp in UTC
#if __cplusplus >= 202002L
  const std::string dateStr = std::format("{:%FT%T%Ez}", timePoint);
#else
  const static int dateToSecondsSize = 19;
  const static int millisecondsSize  = 8;
  const static int timeZoneSize      = 1;
  const static int dateSize          = dateToSecondsSize + millisecondsSize + timeZoneSize;

  std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
  std::tm tm       = *std::gmtime(&time);

  char bufferDate[dateSize];  // example: 2024-10-18T07:26:00.123456Z
  std::strftime(bufferDate, sizeof(bufferDate), "%Y-%m-%dT%H:%M:%S", &tm);
  auto microseconds =
      std::chrono::duration_cast<std::chrono::microseconds>(timePoint.time_since_epoch()) %
      std::chrono::seconds(1);

  char bufferMilliseconds[millisecondsSize];
  std::snprintf(bufferMilliseconds, sizeof(bufferMilliseconds), ".%06ld",
                static_cast<long>(microseconds.count()));

  std::strcat(bufferDate, bufferMilliseconds);
  std::strcat(bufferDate, "Z");

  const std::string dateStr(bufferDate);
#endif

  json_["@timestamp"] = dateStr;
}

void ElasticSearchRecordable::SetObservedTimestamp(
    opentelemetry::common::SystemTimestamp timestamp) noexcept
{
  json_["observedtimestamp"] = timestamp.time_since_epoch().count();
}

void ElasticSearchRecordable::SetSeverity(opentelemetry::logs::Severity severity) noexcept
{
  auto &severityField = json_["log"]["level"];

  // Convert the severity enum to a string
  std::uint32_t severity_index = static_cast<std::uint32_t>(severity);
  if (severity_index >= std::extent<decltype(opentelemetry::logs::SeverityNumToText)>::value)
  {
    std::stringstream sout;
    sout << "Invalid severity(" << severity_index << ")";
    severityField = sout.str();
  }
  else
  {
    severityField = opentelemetry::logs::SeverityNumToText[severity_index];
  }
}

void ElasticSearchRecordable::SetBody(const opentelemetry::common::AttributeValue &message) noexcept
{
  WriteValue(message, "message");
}

void ElasticSearchRecordable::SetTraceId(const opentelemetry::trace::TraceId &trace_id) noexcept
{
  if (trace_id.IsValid())
  {
    char trace_buf[32];
    trace_id.ToLowerBase16(trace_buf);
    json_["traceid"] = std::string(trace_buf, sizeof(trace_buf));
  }
  else
  {
    json_.erase("traceid");
  }
}

void ElasticSearchRecordable::SetSpanId(const opentelemetry::trace::SpanId &span_id) noexcept
{
  if (span_id.IsValid())
  {
    char span_buf[16];
    span_id.ToLowerBase16(span_buf);
    json_["spanid"] = std::string(span_buf, sizeof(span_buf));
  }
  else
  {
    json_.erase("spanid");
  }
}

void ElasticSearchRecordable::SetTraceFlags(
    const opentelemetry::trace::TraceFlags &trace_flags) noexcept
{
  char flag_buf[2];
  trace_flags.ToLowerBase16(flag_buf);
  json_["traceflags"] = std::string(flag_buf, sizeof(flag_buf));
}

void ElasticSearchRecordable::SetAttribute(
    nostd::string_view key,
    const opentelemetry::common::AttributeValue &value) noexcept
{
  WriteValue(value, key.data());
}

void ElasticSearchRecordable::SetResource(
    const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  for (auto &attribute : resource.GetAttributes())
  {
    WriteValue(attribute.second, attribute.first);
  }
}

void ElasticSearchRecordable::SetInstrumentationScope(
    const opentelemetry::sdk::instrumentationscope::InstrumentationScope
        &instrumentation_scope) noexcept
{
  json_["log"]["logger"] = instrumentation_scope.GetName();
}

}  // namespace logs
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
