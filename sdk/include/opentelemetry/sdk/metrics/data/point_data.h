// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/sdk/common/attribute_utils.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using PointAttributes = opentelemetry::sdk::common::AttributeMap;

/**
 * A point in the "Metric stream" data model.
 */

struct BasePointData
{
  opentelemetry::common::SystemTimestamp start_epoch_nanos_;
  opentelemetry::common::SystemTimestamp end_epoch_nanos_;
  PointAttributes attributes_;
};

class PointData
{};

struct LongSingularPointData : public PointData
{
  LongSingularPointData(long value) : value_(value) {}
  long value_;
};

struct DoubleSingularPointData : public PointData
{
  DoubleSingularPointData(long value) : value_(value) {}
  long value_;
};

struct LongHistogramPointData : public PointData
{
  LongHistogramPointData(std::vector<long> &boundaries)
      : boundaries_{boundaries}, counts_(boundaries.size() + 1, 0), sum_(0), count_(0)
  {}
  std::vector<long> boundaries_;
  std::vector<uint64_t> counts_;
  long sum_;
  uint64_t count_;
};

struct DoubleHistogramPointData : public PointData
{
  DoubleHistogramPointData(std::vector<double> &boundaries)
      : boundaries_{boundaries}, counts_(boundaries.size() + 1, 0), sum_(0), count_(0)
  {}
  std::vector<double> boundaries_;
  std::vector<uint64_t> counts_;
  double sum_;
  uint64_t count_;
};

struct DropPointData : public PointData
{};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif

/*class PointData {
public:
    public:
    PointData(opentelemetry::common::SystemTimestamp& start_epoch_nanos,
              opentelemetry::common::SystemTimestamp& epoch_nanos,
              PointAttributes& attributes): start_epoch_nanos_(start_epoch_nanos),
              epoch_nanos_(epoch_nanos), epoch_nanos_{attributes}
    {}
    //Returns the start epoch timestamp
    opentelemetry::common::SystemTimestamp GetStartEpochNanos() noexcept
    {
        return start_epoch_nanos_;
    }

    //Returns the epoch timestamp in nanos when data were collected
    opentelemetry::common::SystemTimestamp GetEpochNanos() noexcept
    {
        return epoch_nanos_;
    }

    //Returns the attributes associated with this Point
    PointAttributes& GetAttributes() noexcept
    {
        return attributes_;
    }

    private:
        opentelemetry::common::SystemTimestamp start_epoch_nanos_;
        opentelemetry::common::SystemTimestamp epoch_nanos_;
        PointAttributes attributes_;
};

template<class T>
class SingularPointData: public PointData {
    public:
    SingularPointData(opentelemetry::common::SystemTimestamp& start_epoch_nanos,
                      opentelemetry::common::SystemTimestamp& epoch_nanos,
                      PointAttributes& attributes,
                      T value):
                     PointData(start_epoch_nanos, epoch_nanos, attributes),
                     value_(value)
                     {}

    T GetValue() const noexcept {
        return value_;
    }

    private:
        T value_;
};

template<class T>
class SummaryPointData: public PointData {
    public:
    SummaryPointData(opentelemetry::common::SystemTimestamp& start_epoch_nanos,
                      opentelemetry::common::SystemTimestamp& epoch_nanos,
                      PointAttributes& attributes,
                      uint64_t count,
                      T sum):
                     PointData(start_epoch_nanos, epoch_nanos, attributes),
                     sum_(sum), count_(count)
                     {}
    T GetSum() const noexcept {
        return sum_;
    }

    uint64_t GetCount() const noexcept {
        return count_;
    }

    private:
        T sum_;
        uint64_t count_;
};

template <class T>
class HistogramPointData : public PointData {
    HistogramPointData(opentelemetry::common::SystemTimestamp& start_epoch_nanos,
                        opentelemetry::common::SystemTimestamp& epoch_nanos,
                        PointAttributes& attributes,
                        T sum,
                        std::vector<T> boundaries,
                        std::vector<uint64_t> counts):
                    PointData(start_epoch_nanos, epoch_nanos, attributes),
                    sum_(sum), boundaries_{boundaries}, counts_{count}
                    {}
    T GetSum() const noexcept {
        return sum_;
    }
    std::vector<T> GetBoundaries() {
        return boundaries_;
    }
    std::vector<uint64_t> GetCounts() {
        return counts_;
    }
    private:
        T sum_;
        std::vector<T> boundaries_;
        std::vector<uint64_t> counts_;
};*/
//}  // namespace metrics
//}  // namespace sdk
// OPENTELEMETRY_END_NAMESPACE
//#endif