#pragma once

#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/variant.h"
#include <vector>

enum AggregatorKind
{
  Counter = 0,
  MinMaxSumCount = 1,
  Gauge = 2,
  Sketch = 3,
  Histogram = 4,
  Exact = 5,
};

OPENTELEMETRY_BEGIN_NAMESPACE

namespace sdk
{
namespace metrics
{
class Record
{
  using RecordValue = nostd::variant<std::vector<short>,
                                     std::vector<int>,
                                     std::vector<float>,
                                     std::vector<double>>;
public:
  explicit Record(std::string name, std::string description,
                  AggregatorKind aggregatorKind,
                  std::string labels,
                  RecordValue value,
                  core::SystemTimestamp timestamp = core::SystemTimestamp(std::chrono::system_clock::now()))
  {
    name_ = name;
    description_ = description;
    aggregatorKind_ = aggregatorKind;
    labels_ = labels;
    value_ = value;
    timestamp_ = timestamp;
  }

  template<typename T>
  void SetValue(std::vector<T> value)
  {
    value_ = value;
  }

  std::string GetName() {return name_;}
  std::string GetDescription() {return description_;}
  AggregatorKind GetAggregatorKind() {return aggregatorKind_;}
  std::string GetLabels() {return labels_;}
  RecordValue GetValue() {return value_;}
  core::SystemTimestamp GetTimestamp() {return timestamp_;}

private:
  std::string name_;
  std::string description_;
  AggregatorKind aggregatorKind_;
  std::string labels_;
  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> value_;
  core::SystemTimestamp timestamp_;
};
} // namespace metrics
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE