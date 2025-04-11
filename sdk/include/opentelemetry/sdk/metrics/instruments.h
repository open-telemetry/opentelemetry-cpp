// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <algorithm>
#include <cctype>
#include <functional>
#include <string>

#include "opentelemetry/sdk/metrics/state/filtered_ordered_attribute_map.h"
#include "opentelemetry/version.h"

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
  kObservableUpDownCounter,
  kGauge
};

enum class InstrumentClass
{
  kSync,
  kAsync
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

inline bool CaseInsensitiveEquals(const std::string &lhs, const std::string &rhs) noexcept
{
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin(), [](char a, char b) {
           return std::tolower(static_cast<unsigned char>(a)) ==
                  std::tolower(static_cast<unsigned char>(b));
         });
}

inline bool IsInstrumentDuplicate(const InstrumentDescriptor &lhs,
                                  const InstrumentDescriptor &rhs) noexcept
{
  const bool names_match        = CaseInsensitiveEquals(lhs.name_, rhs.name_);
  const bool kinds_match        = (lhs.type_ == rhs.type_) && (lhs.value_type_ == rhs.value_type_);
  const bool units_match        = (lhs.unit_ == rhs.unit_);
  const bool descriptions_match = (lhs.description_ == rhs.description_);

  return names_match && (!kinds_match || !units_match || !descriptions_match);
}

struct InstrumentEqualNameCaseInsensitive
{
  bool operator()(const InstrumentDescriptor &lhs, const InstrumentDescriptor &rhs) const noexcept
  {
    const bool names_match = CaseInsensitiveEquals(lhs.name_, rhs.name_);
    const bool kinds_match = (lhs.type_ == rhs.type_) && (lhs.value_type_ == rhs.value_type_);
    const bool units_match = (lhs.unit_ == rhs.unit_);
    const bool descriptions_match = (lhs.description_ == rhs.description_);

    return names_match && kinds_match && units_match && descriptions_match;
  }
};

struct InstrumentDescriptorHash
{
  std::size_t operator()(const InstrumentDescriptor &instrument_descriptor) const noexcept
  {
    std::size_t hashcode{};

    for (char c : instrument_descriptor.name_)
    {
      sdk::common::GetHash(hashcode,
                           static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    sdk::common::GetHash(hashcode, instrument_descriptor.description_);
    sdk::common::GetHash(hashcode, instrument_descriptor.unit_);
    sdk::common::GetHash(hashcode, static_cast<uint32_t>(instrument_descriptor.type_));
    sdk::common::GetHash(hashcode, static_cast<uint32_t>(instrument_descriptor.value_type_));
    return hashcode;
  }
};

using MetricAttributes               = opentelemetry::sdk::metrics::FilteredOrderedAttributeMap;
using MetricAttributesHash           = opentelemetry::sdk::metrics::FilteredOrderedAttributeMapHash;
using AggregationTemporalitySelector = std::function<AggregationTemporality(InstrumentType)>;

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
