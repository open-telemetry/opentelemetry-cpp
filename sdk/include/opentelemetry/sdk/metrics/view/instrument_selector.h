// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class InstrumentSelector
{
public:
  InstrumentSelector(opentelemetry::sdk::metrics::InstrumentType instrument_type,
                     opentelemetry::nostd::string_view name)
      : name_filter_{new PatternPredicate(name)}, instrument_type_{instrument_type}
  {}

  // Returns name filter predicate. This shouldn't be deleted
  const Predicate *const GetNameFilter() { return name_filter_.get(); }

  // Returns instrument filter.
  InstrumentType GetInstrumentType() { return instrument_type_; }

private:
  InstrumentType instrument_type_;
  std::unique_ptr<Predicate> name_filter_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif