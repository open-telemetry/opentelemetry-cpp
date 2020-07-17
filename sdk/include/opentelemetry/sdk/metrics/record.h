#pragma once

#include <map>
#include "opentelemetry/core/timestamp.h"
#include <string>
#include <vector>


OPENTELEMETRY_BEGIN_NAMESPACE

//namespace metrics_api = opentelemetry::metrics;

namespace sdk
{
namespace metrics
{
class Record
{
public:
  explicit Record(std::string name, std::string description,
                  std::string labels,
                  std::vector<int> value,
                  core::SystemTimestamp timestamp = core::SystemTimestamp(std::chrono::system_clock::now()))
  {
    name_ = name;
    description_ = description;
    labels_ = labels;
    value_ = value;
    timestamp_ = timestamp;
  }
 
  std::string GetName() {return name_;}
  std::string GetDescription() {return description_;}
  std::string GetLabels() {return labels_;}
  std::vector<int> GetValue() {return value_;}
  core::SystemTimestamp GetTimestamp() {return timestamp_;}

private:
  std::string name_;
  std::string description_;
  std::string labels_;
  std::vector<int> value_;
  core::SystemTimestamp timestamp_;
};

} // namespace metrics
} // namespace sdk

OPENTELEMETRY_END_NAMESPACE
