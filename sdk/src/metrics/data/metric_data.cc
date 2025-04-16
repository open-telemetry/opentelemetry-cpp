// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace
{

template <typename T>
void PrintTo(T value, std::ostream *os)
{
  *os << "\"" << std::to_string(value) << "\"";
}

void PrintTo(bool value, std::ostream *os)
{
  *os << (value ? "true" : "false");
}

void PrintTo(const std::string &value, std::ostream *os)
{
  *os << value;
}

template <typename T>
void PrintTo(const std::vector<T> &list, std::ostream *os)
{
  *os << "[";
  size_t size = list.size();
  for (size_t i = 0; i < size; ++i)
  {
    PrintTo(list[i], os);
    if (i != size - 1)
    {
      *os << ", ";
    }
  }
  *os << "]";
}

void PrintTo(const opentelemetry::sdk::common::OwnedAttributeValue &value, std::ostream *os)
{
  nostd::visit([&](const auto &value) { PrintTo(value, os); }, value);
}

void PrintTo(const opentelemetry::sdk::metrics::PointAttributes &point_attributes, std::ostream *os)
{
  *os << "{";
  for (const auto &pair : point_attributes)
  {
    *os << "{";
    PrintTo(pair.first, os);
    *os << ", ";
    PrintTo(pair.second, os);
    *os << "}";
  }
  *os << "}";
}

void PrintTo(const opentelemetry::sdk::metrics::ValueType &value, std::ostream *os)
{
  nostd::visit([&](const auto &value) { return PrintTo(value, os); }, value);
}

class PointTypeVisitor
{
public:
  PointTypeVisitor(std::ostream *os) : os_(os) {}

  void operator()(const opentelemetry::sdk::metrics::SumPointData &point)
  {
    *os_ << "{value = ";
    PrintTo(point.value_, os_);
    *os_ << "], is_monotonic = ";
    PrintTo(point.is_monotonic_, os_);
    *os_ << "}";
  }

  void operator()(const opentelemetry::sdk::metrics::LastValuePointData &point)
  {
    *os_ << "{value = ";
    PrintTo(point.value_, os_);
    *os_ << "], is_lastvalue_valid = ";
    PrintTo(point.is_lastvalue_valid_, os_);
    *os_ << ", sample_ts = ";
    PrintTo(point.sample_ts_.time_since_epoch().count(), os_);
    *os_ << "ns}";
  }

  void operator()(const opentelemetry::sdk::metrics::HistogramPointData &point)
  {
    *os_ << "{boundaries = ";
    PrintTo(point.boundaries_, os_);
    *os_ << "], sum = ";
    PrintTo(point.sum_, os_);
    *os_ << ", min = ";
    PrintTo(point.min_, os_);
    *os_ << ", max = ";
    PrintTo(point.max_, os_);
    *os_ << ", counts = ";
    PrintTo(point.counts_, os_);
    *os_ << ", count = " << point.count_ << ", record_min_max = ";
    return PrintTo(point.record_min_max_, os_);
  }

  void operator()(const opentelemetry::sdk::metrics::DropPointData & /* point */)
  {
    *os_ << "<DropPointData>";
  }

private:
  std::ostream *os_;
};

void PrintTo(const opentelemetry::sdk::metrics::PointType &point_type, std::ostream *os)
{
  nostd::visit(PointTypeVisitor(os), point_type);
}

}  // namespace

void PrintTo(const PointDataAttributes &point_data_attributes, std::ostream *os)
{
  *os << "{attributes = ";
  PrintTo(point_data_attributes.attributes, os);
  *os << ", point_data = ";
  PrintTo(point_data_attributes.point_data, os);
  *os << "}";
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
