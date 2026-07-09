// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/instrumentationscope/instrumentation_scope.h"
#include "opentelemetry/sdk/resource/resource.h"
#include "opentelemetry/sdk/trace/span_data.h"
#include "opentelemetry/sdk/trace/span_limits.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

namespace
{

// Populate from attributes enforcing count and value-length limits.
opentelemetry::sdk::common::AttributeMap PopulateAttributeMap(
    const opentelemetry::common::KeyValueIterable &attributes,
    std::uint32_t count_limit,
    std::size_t value_length_limit)
{
  opentelemetry::sdk::common::AttributeMap map;
  map.reserve((std::min)(static_cast<std::size_t>(count_limit), attributes.size()));

  // Insert attributes until the count limit of unique keys is reached.
  std::uint32_t inserted = 0;
  attributes.ForEachKeyValue(
      [count_limit, value_length_limit, &map, &inserted](
          nostd::string_view key, const opentelemetry::common::AttributeValue &value) noexcept {
        if (inserted >= count_limit)
        {
          return false;  // The attribute count limit has been reached. Stop iterating.
        }

        std::pair<opentelemetry::sdk::common::OwnedAttributeValue, bool> convert_result =
            opentelemetry::sdk::common::VisitVariant(
                opentelemetry::sdk::common::AttributeConverter(value_length_limit), value);
        if (!convert_result.second)
        {
          return true;  // There was an exception converting. Skip this attribute and continue.
        }

        auto result = opentelemetry::sdk::common::AttributeInsertOrAssign(
            map, key, std::move(convert_result.first));
        if (result.second)
        {
          ++inserted; // A unique key was inserted.  
        }
        return true;
      });
  return map;
}

std::uint32_t CountDroppedAttributes(const opentelemetry::common::KeyValueIterable &attributes,
                                     std::uint32_t count_limit)
{
  const std::size_t total = attributes.size();
  return static_cast<std::uint32_t>(total > count_limit ? total - count_limit : 0);
}

}  // namespace

SpanDataEvent::SpanDataEvent(std::string name,
                             opentelemetry::common::SystemTimestamp timestamp,
                             const opentelemetry::common::KeyValueIterable &attributes,
                             std::uint32_t attribute_count_limit,
                             std::size_t attribute_value_length_limit)
    : name_(std::move(name)),
      timestamp_(timestamp),
      attribute_map_(
          PopulateAttributeMap(attributes, attribute_count_limit, attribute_value_length_limit)),
      dropped_attributes_count_(CountDroppedAttributes(attributes, attribute_count_limit))
{}

const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> &
SpanDataEvent::GetAttributes() const noexcept
{
  return attribute_map_.GetAttributes();
}

SpanDataLink::SpanDataLink(opentelemetry::trace::SpanContext span_context,
                           const opentelemetry::common::KeyValueIterable &attributes,
                           std::uint32_t attribute_count_limit,
                           std::size_t attribute_value_length_limit)
    : span_context_(std::move(span_context)),
      attribute_map_(
          PopulateAttributeMap(attributes, attribute_count_limit, attribute_value_length_limit)),
      dropped_attributes_count_(CountDroppedAttributes(attributes, attribute_count_limit))
{}

const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> &
SpanDataLink::GetAttributes() const noexcept
{
  return attribute_map_.GetAttributes();
}

const opentelemetry::sdk::resource::Resource &SpanData::GetResource() const noexcept
{
  if (resource_ == nullptr)
  {
    // this shouldn't happen as TraceProvider provides default resources
    static opentelemetry::sdk::resource::Resource resource =
        opentelemetry::sdk::resource::Resource::GetEmpty();
    return resource;
  }
  return *resource_;
}

const opentelemetry::sdk::trace::InstrumentationScope &SpanData::GetInstrumentationScope()
    const noexcept
{
  if (instrumentation_scope_ == nullptr)
  {
    // this shouldn't happen as Tracer ensures there is valid default instrumentation scope.
    static std::unique_ptr<opentelemetry::sdk::instrumentationscope::InstrumentationScope>
        instrumentation_scope =
            opentelemetry::sdk::instrumentationscope::InstrumentationScope::Create(
                "unknown_service");
    return *instrumentation_scope;
  }
  return *instrumentation_scope_;
}

const std::unordered_map<std::string, opentelemetry::sdk::common::OwnedAttributeValue> &
SpanData::GetAttributes() const noexcept
{
  return attribute_map_.GetAttributes();
}

void SpanData::SetIdentity(const opentelemetry::trace::SpanContext &span_context,
                           opentelemetry::trace::SpanId parent_span_id) noexcept
{
  span_context_   = span_context;
  parent_span_id_ = parent_span_id;
}

void SpanData::SetAttribute(nostd::string_view key,
                            const opentelemetry::common::AttributeValue &value) noexcept
{
  if (attribute_map_.size() >= limits_.attribute_count_limit)
  {
    // The map is at the limit. Can only update existing keys.
    auto it = attribute_map_.find(std::string(key));
    if (it == attribute_map_.end())
    {
      ++dropped_attributes_count_;
      return;
    }
    // try to convert the value.
    std::pair<opentelemetry::sdk::common::OwnedAttributeValue, bool> convert_result =
        opentelemetry::sdk::common::VisitVariant(
            opentelemetry::sdk::common::AttributeConverter(limits_.attribute_value_length_limit),
            value);
    if (!convert_result.second)
    {
      return;  // There was an exception converting the value. Skip this attribute.
    }
    it->second = std::move(convert_result.first);
    return;
  }

  // The map is under the limit. Set the attribute (insert or assign).
  attribute_map_.SetAttribute(key, value, limits_.attribute_value_length_limit);
}

void SpanData::AddEvent(nostd::string_view name,
                        opentelemetry::common::SystemTimestamp timestamp,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (events_.size() >= limits_.event_count_limit)
  {
    ++dropped_events_count_;
    return;
  }
  events_.emplace_back(std::string(name), timestamp, attributes,
                       limits_.event_attribute_count_limit, limits_.attribute_value_length_limit);
}

void SpanData::AddLink(const opentelemetry::trace::SpanContext &span_context,
                       const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (links_.size() >= limits_.link_count_limit)
  {
    ++dropped_links_count_;
    return;
  }
  links_.emplace_back(span_context, attributes, limits_.link_attribute_count_limit,
                      limits_.attribute_value_length_limit);
}

void SpanData::SetSpanLimits(const SpanLimits &limits) noexcept
{
  limits_ = limits;
}

void SpanData::SetStatus(opentelemetry::trace::StatusCode code,
                         nostd::string_view description) noexcept
{
  status_code_ = code;
  status_desc_ = std::string(description);
}

void SpanData::SetName(nostd::string_view name) noexcept
{
  name_ = std::string(name.data(), name.length());
}

void SpanData::SetTraceFlags(opentelemetry::trace::TraceFlags flags) noexcept
{
  flags_ = flags;
}

void SpanData::SetSpanKind(opentelemetry::trace::SpanKind span_kind) noexcept
{
  span_kind_ = span_kind;
}

void SpanData::SetResource(const opentelemetry::sdk::resource::Resource &resource) noexcept
{
  resource_ = &resource;
}

void SpanData::SetStartTime(opentelemetry::common::SystemTimestamp start_time) noexcept
{
  start_time_ = start_time;
}

void SpanData::SetDuration(std::chrono::nanoseconds duration) noexcept
{
  duration_ = duration;
}

void SpanData::SetInstrumentationScope(const InstrumentationScope &instrumentation_scope) noexcept
{
  instrumentation_scope_ = &instrumentation_scope;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
