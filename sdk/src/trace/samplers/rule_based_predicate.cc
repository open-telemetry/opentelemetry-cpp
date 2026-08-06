// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/trace/samplers/rule_based_predicate.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/version.h"

#include "src/common/wildcard_match.h"

namespace
{

using opentelemetry::nostd::function_ref;
using opentelemetry::sdk::common::WildcardMatch;
namespace nostd = opentelemetry::nostd;

// Checks every string form of an attribute value; array values match if any item matches.
class ValueMatcher
{
public:
  explicit ValueMatcher(function_ref<bool(const std::string &)> check) : check_(check) {}

  bool operator()(bool v) const { return check_(v ? "true" : "false"); }
  bool operator()(int32_t v) const { return check_(std::to_string(v)); }
  bool operator()(int64_t v) const { return check_(std::to_string(v)); }
  bool operator()(uint32_t v) const { return check_(std::to_string(v)); }
  bool operator()(uint64_t v) const { return check_(std::to_string(v)); }
  bool operator()(double v) const { return check_(std::to_string(v)); }
  bool operator()(const char *v) const { return v != nullptr && check_(std::string(v)); }
  bool operator()(nostd::string_view v) const { return check_(std::string(v)); }
  // Array values match when any item matches.
  template <typename T>
  bool operator()(nostd::span<const T> v) const
  {
    for (const auto &value : v)
    {
      if ((*this)(value))
      {
        return true;
      }
    }
    return false;
  }
  // Byte arrays have no string form to match.
  bool operator()(nostd::span<const uint8_t> /* v */) const { return false; }

private:
  function_ref<bool(const std::string &)> check_;
};

bool AttributeMatches(const opentelemetry::common::KeyValueIterable &attributes,
                      const std::string &key,
                      function_ref<bool(const std::string &)> check) noexcept
{
  bool matched = false;
  attributes.ForEachKeyValue(
      [&](nostd::string_view attr_key, opentelemetry::common::AttributeValue value) {
        if (attr_key != key)
        {
          return true;
        }
        matched = nostd::visit(ValueMatcher(check), value);
        return false;
      });
  return matched;
}

}  // namespace

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{

RuleBasedPredicate::RuleBasedPredicate(RuleBasedPredicateOptions options)
    : options_(std::move(options))
{}

bool RuleBasedPredicate::SpanMatches(
    const opentelemetry::trace::SpanContext &parent_context,
    nostd::string_view /* name */,
    opentelemetry::trace::SpanKind span_kind,
    const opentelemetry::common::KeyValueIterable &attributes,
    const opentelemetry::trace::SpanContextKeyValueIterable & /* links */) const noexcept
{
  if (!ParentMatches(parent_context))
  {
    return false;
  }
  if (!SpanKindMatches(span_kind))
  {
    return false;
  }
  if (options_.match_values &&
      !AttributeMatches(attributes, options_.values_key, [this](const std::string &candidate) {
        return std::find(options_.values.begin(), options_.values.end(), candidate) !=
               options_.values.end();
      }))
  {
    return false;
  }
  if (options_.match_patterns &&
      !AttributeMatches(attributes, options_.patterns_key, [this](const std::string &candidate) {
        for (const auto &pattern : options_.excluded)
        {
          if (WildcardMatch(pattern, candidate))
          {
            return false;
          }
        }
        if (options_.included.empty())
        {
          return true;
        }
        for (const auto &pattern : options_.included)
        {
          if (WildcardMatch(pattern, candidate))
          {
            return true;
          }
        }
        return false;
      }))
  {
    return false;
  }
  return true;
}

nostd::string_view RuleBasedPredicate::GetDescription() const noexcept
{
  return "RuleBasedPredicate";
}

bool RuleBasedPredicate::ParentMatches(
    const opentelemetry::trace::SpanContext &parent_context) const noexcept
{
  if (!(options_.match_parent_none || options_.match_parent_remote || options_.match_parent_local))
  {
    return true;
  }
  if (!parent_context.IsValid())
  {
    return options_.match_parent_none;
  }
  return parent_context.IsRemote() ? options_.match_parent_remote : options_.match_parent_local;
}

bool RuleBasedPredicate::SpanKindMatches(opentelemetry::trace::SpanKind span_kind) const noexcept
{
  if (!(options_.match_span_kind_internal || options_.match_span_kind_server ||
        options_.match_span_kind_client || options_.match_span_kind_producer ||
        options_.match_span_kind_consumer))
  {
    return true;
  }
  switch (span_kind)
  {
    case opentelemetry::trace::SpanKind::kInternal:
      return options_.match_span_kind_internal;
    case opentelemetry::trace::SpanKind::kServer:
      return options_.match_span_kind_server;
    case opentelemetry::trace::SpanKind::kClient:
      return options_.match_span_kind_client;
    case opentelemetry::trace::SpanKind::kProducer:
      return options_.match_span_kind_producer;
    case opentelemetry::trace::SpanKind::kConsumer:
      return options_.match_span_kind_consumer;
  }
  return false;
}

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
