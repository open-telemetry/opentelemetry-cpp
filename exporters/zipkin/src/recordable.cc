#include "opentelemetry/exporters/zipkin/recordable.h"

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace zipkin
{

const int kAttributeValueSize = 14;
const char *EMPTY_PARENT_SPAN = "0000000000000000";

void Recordable::SetIds(trace::TraceId trace_id,
                        trace::SpanId span_id,
                        trace::SpanId parent_span_id) noexcept
{
  char traceid[trace::TraceId::kSize * 2] = {0};
  trace_id.ToLowerBase16(traceid);
  char spanid[trace::SpanId::kSize * 2] = {0};
  span_id.ToLowerBase16(spanid);
  char parentid[trace::SpanId::kSize * 2] = {0};
  parent_span_id.ToLowerBase16(parentid);
  span_["id"]       = std::string(spanid, 16);
  span_["parentId"] = std::string(parentid, 16);
  span_["traceId"]  = std::string(traceid, 32);
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

  // attribute->set_key(key.data(), key.size());

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
  nlohmann::json annotations = nlohmann::json::object();  // empty object
  attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
    PopulateAttribute(annotations, key, value);
    return true;
  });
  span_["annotations"][name.data()]["value"] = annotations;
  span_["annotations"]["timestamp"] =
      std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
}

void Recordable::AddLink(const opentelemetry::trace::SpanContext &span_context,
                         const common::KeyValueIterable &attributes) noexcept
{
  // TODO: Populate once supported
  // https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/trace/sdk_exporters/zipkin.md
}

void Recordable::SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept
{
  span_["tags"]["otel.status_code"] = code;
  if (description.size())
    span_["tags"]["otel.status_description"] = description;
}

void Recordable::SetName(nostd::string_view name) noexcept
{
  span_["name"] = name.data();
  // span_.set_name(name.data(), name.size());
}

void Recordable::SetStartTime(opentelemetry::core::SystemTimestamp start_time) noexcept
{
  span_["timestamp"] = start_time.time_since_epoch().count();
  // span_.set_start_time_unix_nano(start_time.time_since_epoch().count());
}

void Recordable::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  span_["duration"] = duration.count();
  /*const uint64_t unix_end_time = span_.start_time_unix_nano() + duration.count();
  span_.set_end_time_unix_nano(unix_end_time);*/
}
}  // namespace zipkin
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
