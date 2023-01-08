/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "opentelemetry/opentracingshim/span_context_shim.h"

#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/baggage/baggage_context.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/trace/semantic_conventions.h"
#include "opentelemetry/trace/span_context_kv_iterable.h"
#include "opentelemetry/trace/tracer.h"
#include "opentracing/propagation.h"
#include "opentracing/tracer.h"
#include "opentracing/value.h"

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim::utils
{

static inline opentelemetry::common::AttributeValue attributeFromValue(const opentracing::Value& value)
{
  using opentelemetry::common::AttributeValue;

  static struct
  {
    AttributeValue operator()(bool v) { return v; }
    AttributeValue operator()(double v) { return v; }
    AttributeValue operator()(int64_t v) { return v; }
    AttributeValue operator()(uint64_t v) { return v; }
    AttributeValue operator()(const std::string& v) { return v.c_str(); }
    AttributeValue operator()(opentracing::string_view v) { return nostd::string_view{v.data()}; }
    AttributeValue operator()(std::nullptr_t) { return nostd::string_view{}; }
    AttributeValue operator()(const char* v) { return v; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Values>) { return nostd::string_view{}; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>) { return nostd::string_view{}; }
  } AttributeMapper;

  return opentracing::Value::visit(value, AttributeMapper);
}

static inline std::string stringFromValue(const opentracing::Value& value)
{
  static struct
  {
    std::string operator()(bool v) { return v ? "true" : "false"; }
    std::string operator()(double v) { return std::to_string(v); }
    std::string operator()(int64_t v) { return std::to_string(v); }
    std::string operator()(uint64_t v) { return std::to_string(v); }
    std::string operator()(const std::string& v) { return v; }
    std::string operator()(opentracing::string_view v) { return std::string{v.data()}; }
    std::string operator()(std::nullptr_t) { return std::string{}; }
    std::string operator()(const char* v) { return std::string{v}; }
    std::string operator()(opentracing::util::recursive_wrapper<opentracing::Values>) { return std::string{}; }
    std::string operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>) { return std::string{}; }
  } StringMapper;

  return opentracing::Value::visit(value, StringMapper);
}

static inline bool isBaggageEmpty(const nostd::shared_ptr<opentelemetry::baggage::Baggage>& baggage)
{
  if (baggage)
  {
    return baggage->GetAllEntries([](nostd::string_view, nostd::string_view){
      return false;
    });
  }

  return true;
}

static opentelemetry::trace::StartSpanOptions makeOptionsShim(const opentracing::StartSpanOptions& options) noexcept
{
  using opentracing::SpanReferenceType;
  // If an explicit start timestamp is specified, a conversion MUST
  // be done to match the OpenTracing and OpenTelemetry units.
  opentelemetry::trace::StartSpanOptions options_shim;
  options_shim.start_system_time = opentelemetry::common::SystemTimestamp{options.start_system_timestamp};
  options_shim.start_steady_time = opentelemetry::common::SteadyTimestamp{options.start_steady_timestamp};

  const auto& refs = options.references;
  // If a list of Span references is specified...
  if (!refs.empty())
  {
    auto first_child_of = std::find_if(refs.cbegin(), refs.cend(),
      [](const std::pair<SpanReferenceType, const opentracing::SpanContext*>& entry){
        return entry.first == SpanReferenceType::ChildOfRef;
      });
    // The first SpanContext with Child Of type in the entire list is used as parent,
    // else the first SpanContext is used as parent
    auto context = (first_child_of != refs.cend()) ? first_child_of->second : refs.cbegin()->second;

    if (auto context_shim = dynamic_cast<const SpanContextShim*>(context))
    {
      options_shim.parent = context_shim->context();
    }
  }

  return options_shim;
}

class LinksIterable final : public opentelemetry::trace::SpanContextKeyValueIterable
{
public:
  using RefsList = std::vector<std::pair<opentracing::SpanReferenceType, const opentracing::SpanContext*>>;
  explicit LinksIterable(const RefsList& refs) noexcept : refs_(refs) {}
  
  bool ForEachKeyValue(nostd::function_ref<bool(opentelemetry::trace::SpanContext, 
                                                const opentelemetry::common::KeyValueIterable&)>
                                                callback) const noexcept override
  {
    using opentracing::SpanReferenceType;
    using namespace opentelemetry::trace::SemanticConventions;
    using LinksList = std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>;
    
    for (const auto& entry : refs_)
    {
      auto context_shim = dynamic_cast<const SpanContextShim*>(entry.second);
      nostd::string_view span_kind;

      if (entry.first == SpanReferenceType::ChildOfRef)
      {
        span_kind = OpentracingRefTypeValues::kChildOf;
      }
      else if (entry.first == SpanReferenceType::FollowsFromRef)
      {
        span_kind = OpentracingRefTypeValues::kFollowsFrom;
      }

      if (context_shim && !span_kind.empty())
      {
        if (!callback(context_shim->context(), 
                      opentelemetry::common::KeyValueIterableView<LinksList>(
                        {{ kOpentracingRefType, span_kind }})))
          return false;
      }
    }

    return true;
  }

  size_t size() const noexcept { return refs_.size(); }

private:
  const RefsList& refs_;
};

static const LinksIterable makeIterableLinks(const opentracing::StartSpanOptions& options) noexcept
{
  return LinksIterable(options.references);
}

class TagsIterable final : public opentelemetry::common::KeyValueIterable
{
public:
  explicit TagsIterable(const std::vector<std::pair<std::string, opentracing::Value>>& tags) noexcept : tags_(tags) {}

  bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)> callback) const noexcept override
  {
    for (const auto& entry : tags_)
    {
      if (!callback(entry.first, utils::attributeFromValue(entry.second))) return false;
    }
    return true;
  }

  size_t size() const noexcept override { return tags_.size(); }

private:
  const std::vector<std::pair<std::string, opentracing::Value>>& tags_;
};

static const TagsIterable makeIterableTags(const opentracing::StartSpanOptions& options) noexcept
{
  return TagsIterable(options.tags);
}

static nostd::shared_ptr<opentelemetry::baggage::Baggage> makeBaggage(const opentracing::StartSpanOptions& options) noexcept
{
  using namespace opentelemetry::baggage;

  std::unordered_map<std::string, std::string> baggage_items;
  // If a list of Span references is specified...
  for (const auto& entry : options.references)
  {
    if (auto context_shim = dynamic_cast<const SpanContextShim*>(entry.second))
    {
      // The union of their Baggage values MUST be used as the initial Baggage of the newly created Span.
      context_shim->ForeachBaggageItem([&baggage_items](const std::string& key, const std::string& value){
        // It is unspecified which Baggage value is used in the case of repeated keys.
        if (baggage_items.find(key) == baggage_items.end())
        {
          baggage_items.emplace(key, value); // Here, only insert if key not already present
        }
        return true;
      });
    }
  }
  // If no such list of references is specified, the current Baggage
  // MUST be used as the initial value of the newly created Span.
  return baggage_items.empty()
    ? GetBaggage(opentelemetry::context::RuntimeContext::GetCurrent())
    : nostd::shared_ptr<Baggage>(new Baggage(baggage_items));
}

} // namespace opentracingshim::utils
OPENTELEMETRY_END_NAMESPACE
