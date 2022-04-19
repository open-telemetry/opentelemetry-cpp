// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/common/attribute_utils.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
enum class InstrumentType
{
  kCounter,
  kHistogram,
  kUpDownCounter,
  kObservableCounter,
  kObservableGauge,
  kObservableUpDownCounter
};

enum class InstrumentValueType
{
  kInt,
  kLong,
  kFloat,
  kDouble
};

enum class AggregationType
{
  kDrop,
  kHistogram,
  kLastValue,
  kSum,
  kDefault
};

enum class AggregationTemporality
{
  kUnspecified,
  kDelta,
  kCumulative
};

struct InstrumentDescriptor
{
  std::string name_;
  std::string description_;
  std::string unit_;
  InstrumentType type_;
  InstrumentValueType value_type_;
};

static inline const char *get_instrument_type(InstrumentType type)
{
  switch (type)
  {
    case InstrumentType::kCounter:
      return "kCounter";
    case InstrumentType::kHistogram:
      return "khistogram";
    case InstrumentType::kObservableCounter:
      return "kObservableCounter";
    case InstrumentType::kObservableGauge:
      return "kObservableGauge";
    case InstrumentType::kObservableUpDownCounter:
      return "kObservableUpDownCounter";
    case InstrumentType::kUpDownCounter:
      return "kUpdownCounter";
    default:
      return "Nothing";
  }
  return "";
}

using MetricAttributes = opentelemetry::sdk::common::OrderedAttributeMap;

/*class InstrumentSelector {
public:
InstrumentSelector(opentelemetry::nostd::string_view name,
opentelemetry::sdk::metrics::InstrumentType type): name_(name.data()), type_(type) {} InstrumentType
GetType(){return type_;} std::string GetNameFilter() { return name_;}

private:
std::string name_;
InstrumentType type_;
};*/
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
