// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/view/predicate.h"
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

enum class PredicateType : uint8_t
{
  kPattern,
  kExact
};

class PredicateFactory
{
public:
  static std::unique_ptr<Predicate> GetPredicate(opentelemetry::nostd::string_view pattern,
                                                 PredicateType type)
  {
    if ((type == PredicateType::kPattern && pattern == "*") ||
        (type == PredicateType::kExact && pattern == ""))
    {
      return std::move(std::unique_ptr<Predicate>(new MatchEverythingPattern()));
    }
    if (type == PredicateType::kPattern)
    {
      return std::move(std::unique_ptr<Predicate>(new PatternPredicate(pattern)));
    }
    if (type == PredicateType::kExact)
    {
      return std::move(std::unique_ptr<Predicate>(new ExactPredicate(pattern)));
    }
    return std::move(std::unique_ptr<Predicate>(new MatchNothingPattern()));
  }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif