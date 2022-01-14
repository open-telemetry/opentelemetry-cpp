// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/sdk/metrics/data/point_data.h"
#  include "opentelemetry/version.h"

#  include <chrono>
#  include <vector>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template <class T>
class Accumulation
{
public:
  virtual void Record(T value) noexcept = 0;
};

template <class T>
class SumAccumulation : public Accumulation<T>
{
public:
  SumAccumulation(T value = 0) : sum_(value) {}

  void Record(T value) noexcept override { sum_ += value; }

  SingularPointData<T> ToPointData() { return SingularPointData<T>(sum_); }

private:
  T sum_;
};

template <class T>
class LastValueAccumulation : public Accumulation<T>
{
public:
  LastValueAccumulation(
      T value                                   = 0,
      opentelemetry::common::SystemTimestamp ts = std::chrono::system_clock::now())
      : last_value_(value), ts_{ts}
  {}

  void Record(T value) noexcept override
  {
    last_value_ = value;
    ts_.Reset(std::chrono::system_clock::now());
  }

  const opentelemetry::common::SystemTimestamp &GetLastValueTimeStamp() const { return ts_; }

  SingularPointData<T> ToPointData() { return SingularPointData<T>(last_value_); }

private:
  T last_value_;
  opentelemetry::common::SystemTimestamp ts_;
};

template <class T>
class HistogramAccumulation : public Accumulation<T>
{
public:
  HistogramAccumulation(std::vector<T> &boundaries) : histogram_(boundaries) {}

  void Record(T value) noexcept override
  {
    histogram_.count_ += 1;
    histogram_.sum_ += value;
    for (auto it = histogram_.boundaries_.begin(); it != histogram_.boundaries_.end(); ++it)
    {
      if (value < *it)
      {
        histogram_.counts_[std::distance(histogram_.boundaries_.begin(), it)] += 1;
        return;
      }
    }
  }

  HistogramPointData<T> &ToPointData() { return histogram_; }

private:
  HistogramPointData<T> histogram_;
};

template <class T>
class NoopAccumulation : public Accumulation<T>
{
public:
  NoopAccumulation() {}

  void Record(T value) noexcept override {}

  NoopPointData<T> ToPointData() { return NoopPointData<T>(); }

private:
  T sum_;
};

template <class T>
struct AccumulationRecord
{
  T accumulation;
  opentelemetry::sdk::common::AttributeMap attributes;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif