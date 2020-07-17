#pragma once

#include <map>
#include "opentelemetry/core/timestamp.h"
#include "opentelemetry/nostd/variant.h"


OPENTELEMETRY_BEGIN_NAMESPACE

//namespace metrics_api = opentelemetry::metrics;
namespace nostd = opentelemetry::nostd;

namespace sdk
{
namespace metrics
{
class Record
{
public:
  explicit Record(std::string name, std::string description,
                  std::map<std::string, std::string> labels,
                  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> value,
                  core::SystemTimestamp timestamp = core::SystemTimestamp(std::chrono::system_clock::now()))
  {
    name_ = name;
    description_ = description;
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
  std::map<std::string, std::string> GetLabels() {return labels_;}
  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> GetValue() {return value_;}
  core::SystemTimestamp GetTimestamp() {return timestamp_;}

private:
  std::string name_;
  std::string description_;
  std::map<std::string, std::string> labels_;
  nostd::variant<std::vector<short>, std::vector<int>, std::vector<float>, std::vector<double>> value_;
  core::SystemTimestamp timestamp_;
};

} // namespace metrics
} // namespace sdk

OPENTELEMETRY_END_NAMESPACE
