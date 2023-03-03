/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "opentelemetry/opentracingshim/span_context_shim.h"

#include "opentelemetry/trace/semantic_conventions.h"
#include "opentelemetry/trace/tracer.h"
#include "opentracing/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim
{
namespace utils
{

static inline opentelemetry::common::AttributeValue attributeFromValue(
    const opentracing::Value &value)
{
  using opentelemetry::common::AttributeValue;

  static struct
  {
    AttributeValue operator()(bool v) { return v; }
    AttributeValue operator()(double v) { return v; }
    AttributeValue operator()(int64_t v) { return v; }
    AttributeValue operator()(uint64_t v) { return v; }
    AttributeValue operator()(const std::string &v) { return nostd::string_view{v}; }
    AttributeValue operator()(opentracing::string_view v) { return nostd::string_view{v.data()}; }
    AttributeValue operator()(std::nullptr_t) { return nostd::string_view{}; }
    AttributeValue operator()(const char *v) { return v; }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Values>)
    {
      return nostd::string_view{};
    }
    AttributeValue operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>)
    {
      return nostd::string_view{};
    }
  } AttributeMapper;

  return opentracing::Value::visit(value, AttributeMapper);
}

static inline std::string stringFromValue(const opentracing::Value &value)
{
  static struct
  {
    std::string operator()(bool v) { return v ? "true" : "false"; }
    std::string operator()(double v) { return std::to_string(v); }
    std::string operator()(int64_t v) { return std::to_string(v); }
    std::string operator()(uint64_t v) { return std::to_string(v); }
    std::string operator()(const std::string &v) { return v; }
    std::string operator()(opentracing::string_view v) { return std::string{v.data()}; }
    std::string operator()(std::nullptr_t) { return std::string{}; }
    std::string operator()(const char *v) { return std::string{v}; }
    std::string operator()(opentracing::util::recursive_wrapper<opentracing::Values>)
    {
      return std::string{};
    }
    std::string operator()(opentracing::util::recursive_wrapper<opentracing::Dictionary>)
    {
      return std::string{};
    }
  } StringMapper;

  return opentracing::Value::visit(value, StringMapper);
}

static inline bool isBaggageEmpty(const nostd::shared_ptr<opentelemetry::baggage::Baggage> &baggage)
{
  if (baggage)
  {
    return baggage->GetAllEntries([](nostd::string_view, nostd::string_view) { return false; });
  }

  return true;
}

class LinksIterable final : public opentelemetry::trace::SpanContextKeyValueIterable
{
public:
  using RefsList =
      std::vector<std::pair<opentracing::SpanReferenceType, const opentracing::SpanContext *>>;

  explicit LinksIterable(const RefsList &refs) noexcept : refs_(refs) {}

  bool ForEachKeyValue(nostd::function_ref<bool(opentelemetry::trace::SpanContext,
                                                const opentelemetry::common::KeyValueIterable &)>
                           callback) const noexcept override
  {
    using opentracing::SpanReferenceType;
    using namespace opentelemetry::trace::SemanticConventions;
    using LinksList = std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>;

    for (const auto &entry : refs_)
    {
      nostd::string_view span_kind;

      if (entry.first == SpanReferenceType::ChildOfRef)
      {
        span_kind = OpentracingRefTypeValues::kChildOf;
      }
      else if (entry.first == SpanReferenceType::FollowsFromRef)
      {
        span_kind = OpentracingRefTypeValues::kFollowsFrom;
      }

      auto context_shim = SpanContextShim::extractFrom(entry.second);

      if (context_shim && !span_kind.empty() &&
          !callback(context_shim->context(), opentelemetry::common::KeyValueIterableView<LinksList>(
                                                 {{kOpentracingRefType, span_kind}})))
      {
        return false;
      }
    }

    return true;
  }

  size_t size() const noexcept override { return refs_.size(); }

private:
  const RefsList &refs_;
};

class TagsIterable final : public opentelemetry::common::KeyValueIterable
{
public:
  explicit TagsIterable(
      const std::vector<std::pair<std::string, opentracing::Value>> &tags) noexcept
      : tags_(tags)
  {}

  bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>
                           callback) const noexcept override
  {
    for (const auto &entry : tags_)
    {
      if (!callback(entry.first, utils::attributeFromValue(entry.second)))
        return false;
    }
    return true;
  }

  size_t size() const noexcept override { return tags_.size(); }

private:
  const std::vector<std::pair<std::string, opentracing::Value>> &tags_;
};

opentelemetry::trace::StartSpanOptions makeOptionsShim(
    const opentracing::StartSpanOptions &options) noexcept;
LinksIterable makeIterableLinks(const opentracing::StartSpanOptions &options) noexcept;
TagsIterable makeIterableTags(const opentracing::StartSpanOptions &options) noexcept;
nostd::shared_ptr<opentelemetry::baggage::Baggage> makeBaggage(
    const opentracing::StartSpanOptions &options) noexcept;

}  // namespace utils
}  // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE
