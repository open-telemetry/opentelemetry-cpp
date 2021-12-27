// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/metrics/view/predicate_factory.h"
#ifndef ENABLE_METRICS_PREVIEW
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class MeterSelector
{
public:
  MeterSelector(opentelemetry::nostd::string_view name,
                opentelemetry::nostd::string_view version,
                opentelemetry::nostd::string_view schema)
      : name_filter_{std::move(PredicateFactory::GetPredicate(name, PredicateType::kExact))},
        version_filter_{std::move(PredicateFactory::GetPredicate(version, PredicateType::kExact))},
        schema_filter_{std::move(PredicateFactory::GetPredicate(schema, PredicateType::kExact))}
  {}

  // Returns name filter predicate. This shouldn't be deleted
  const opentelemetry::sdk::metrics::Predicate *const GetNameFilter() { return name_filter_.get(); }

  // Returns version filter predicate. This shouldn't be deleted
  const opentelemetry::sdk::metrics::Predicate *const GetVersionFilter()
  {
    return version_filter_.get();
  }

  // Returns schema filter predicate. This shouldn't be deleted
  const opentelemetry::sdk::metrics::Predicate *const GetSchemaFilter()
  {
    return schema_filter_.get();
  }

private:
  std::unique_ptr<opentelemetry::sdk::metrics::Predicate> name_filter_;
  std::unique_ptr<opentelemetry::sdk::metrics::Predicate> version_filter_;
  std::unique_ptr<opentelemetry::sdk::metrics::Predicate> schema_filter_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif