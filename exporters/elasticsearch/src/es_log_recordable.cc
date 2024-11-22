// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <string>

#include "opentelemetry/exporters/elasticsearch/es_log_recordable.h"
#include "opentelemetry/logs/severity.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"

namespace nlohmann
{
template <>
struct adl_serializer<opentelemetry::sdk::common::OwnedAttributeValue>
{
  static void to_json(json &j, const opentelemetry::sdk::common::OwnedAttributeValue &v)
  {
    opentelemetry::nostd::visit([&j](const auto &value) { j = value; }, v);
  }
};

template <>
struct adl_serializer<opentelemetry::common::AttributeValue>
{
  static void to_json(json &j, const opentelemetry::common::AttributeValue &v)
  {
    opentelemetry::nostd::visit([&j](const auto &value) { j = value; }, v);
  }
};
}  // namespace nlohmann

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace logs
{
void ElasticSearchRecordable::WriteValue(
    const opentelemetry::sdk::common::OwnedAttributeValue &value,
    const std::string &name)
{
  json_[name] = value;
}

void ElasticSearchRecordable::WriteValue(const opentelemetry::common::AttributeValue &value,
                                         const std::string &name)
{
  json_[name] = value;
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
  constexpr auto dateToSecondsSize = sizeof("YYYY-MM-DDTHH:MM:SS") - 1;
  constexpr auto millisecondsSize  = sizeof(".123456") - 1;
  constexpr auto timeZoneSize      = sizeof("Z") - 1;
  constexpr auto dateSize          = dateToSecondsSize + millisecondsSize + timeZoneSize;

  std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
  std::tm tm       = *std::gmtime(&time);
  auto microseconds =
      std::chrono::duration_cast<std::chrono::microseconds>(timePoint.time_since_epoch()) %
      std::chrono::seconds(1);

  char dateStr[dateSize + 1];  // example: 2024-10-18T07:26:00.123456Z
  std::snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02dT%02d:%02d:%02d.%06ldZ", tm.tm_year + 1900,
                tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                static_cast<long>(microseconds.count()));
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
    severityField =
        std::string("Invalid severity(").append(std::to_string(severity_index)).append(")");
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
    char trace_buf[opentelemetry::trace::TraceId::kSize * 2];
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
    char span_buf[opentelemetry::trace::SpanId::kSize * 2];
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
  for (const auto &attribute : resource.GetAttributes())
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
