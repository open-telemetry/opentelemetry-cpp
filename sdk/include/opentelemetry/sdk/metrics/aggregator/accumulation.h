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

class Accumulation
{
public:
  virtual void Record(long value) noexcept   = 0;
  virtual void Record(double value) noexcept = 0;
};

class LongSumAccumulation : public Accumulation
{
public:
  LongSumAccumulation(long value = 0) : sum_(value) {}

  void Record(long value) noexcept override { sum_ += value; }
  void Record(double value) noexcept override {}

  LongSingularPointData ToPointData() { return LongSingularPointData(sum_); }

private:
  long sum_;
};

class DoubleSumAccumulation : public Accumulation
{
public:
  DoubleSumAccumulation(double value = 0) : sum_(value) {}

  void Record(double value) noexcept override { sum_ += value; }
  void Record(long value) noexcept override {}

  DoubleSingularPointData ToPointData() { return DoubleSingularPointData(sum_); }

private:
  double sum_;
};

class LongLastValueAccumulation : public Accumulation
{
public:
  LongLastValueAccumulation(
      long value                                = 0,
      opentelemetry::common::SystemTimestamp ts = std::chrono::system_clock::now())
      : last_value_(value), ts_{ts}
  {}

  void Record(long value) noexcept override
  {
    last_value_ = value;
    ts_.Reset(std::chrono::system_clock::now());
  }

  void Record(double value) noexcept override {}

  const opentelemetry::common::SystemTimestamp &GetLastValueTimeStamp() const { return ts_; }

  LongSingularPointData ToPointData() { return LongSingularPointData(last_value_); }

private:
  long last_value_;
  opentelemetry::common::SystemTimestamp ts_;
};

class DoubleLastValueAccumulation : public Accumulation
{
public:
  DoubleLastValueAccumulation(
      long value                                = 0,
      opentelemetry::common::SystemTimestamp ts = std::chrono::system_clock::now())
      : last_value_(value), ts_{ts}
  {}

  void Record(double value) noexcept override
  {
    last_value_ = value;
    ts_.Reset(std::chrono::system_clock::now());
  }

  void Record(long value) noexcept override {}

  const opentelemetry::common::SystemTimestamp &GetLastValueTimeStamp() const { return ts_; }

  DoubleSingularPointData ToPointData() { return DoubleSingularPointData(last_value_); }

private:
  double last_value_;
  opentelemetry::common::SystemTimestamp ts_;
};

class LongHistogramAccumulation : public Accumulation
{
public:
  LongHistogramAccumulation(std::vector<long> &boundaries) : histogram_(boundaries) {}

  void Record(long value) noexcept override
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

  void Record(double value) noexcept override {}

  LongHistogramPointData &ToPointData() { return histogram_; }

private:
  LongHistogramPointData histogram_;
};

class DoubleHistogramAccumulation : public Accumulation
{
public:
  DoubleHistogramAccumulation(std::vector<double> &boundaries) : histogram_(boundaries) {}

  void Record(double value) noexcept override
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

  void Record(long value) noexcept override {}

  DoubleHistogramPointData &ToPointData() { return histogram_; }

private:
  DoubleHistogramPointData histogram_;
};

class DropAccumulation : public Accumulation
{
public:
  DropAccumulation() {}

  void Record(long value) noexcept override {}
  void Record(double value) noexcept override {}

  DropPointData ToPointData() { return DropPointData(); }
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