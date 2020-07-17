#pragma once

#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/variant.h"
#include <vector>
#include <string>

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
public:
  explicit Record(std::string name, std::string description,
                  std::string labels,
                  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> value,
                  core::SystemTimestamp timestamp = core::SystemTimestamp(std::chrono::system_clock::now()),
                  AggregatorKind aggkind = AggregatorKind::Counter)
  {
    name_ = name;
    description_ = description;
    labels_ = labels;
    value_ = value;
    timestamp_ = timestamp;
    aggkind_ = aggkind;
  }

  template<typename T>
  void SetValue(std::vector<T> value)
  {
    value_ = value;
  }

  std::string GetName() {return name_;}
  std::string GetDescription() {return description_;}
  std::string GetLabels() {return labels_;}
  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> GetValue() {return value_;}
  core::SystemTimestamp GetTimestamp() {return timestamp_;}
  AggregatorKind GetAggKind() {return aggkind_;}

private:
  std::string name_;
  std::string description_;
  std::string labels_;
  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> value_;
  core::SystemTimestamp timestamp_;
  AggregatorKind aggkind_;
};
} // namespace metrics
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE
