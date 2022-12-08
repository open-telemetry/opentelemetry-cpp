// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/aggregation/histogram_aggregation.h"
#include <algorithm>
#include <iomanip>
#include <limits>
#include <memory>
#include "opentelemetry/version.h"

#include <mutex>
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

LongHistogramAggregation::LongHistogramAggregation(const AggregationConfig *aggregation_config)
{
  auto ac = static_cast<const HistogramAggregationConfig *>(aggregation_config);
  if (ac && ac->boundaries_.size())
  {
    point_data_.boundaries_ = ac->boundaries_;
  }
  else
  {
    point_data_.boundaries_ = {0.0,   5.0,   10.0,   25.0,   50.0,   75.0,   100.0,  250.0,
                               500.0, 750.0, 1000.0, 2500.0, 5000.0, 7500.0, 10000.0};
  }

  if (ac)
  {
    record_min_max_ = ac->record_min_max_;
  }
  point_data_.counts_         = std::vector<uint64_t>(point_data_.boundaries_.size() + 1, 0);
  point_data_.sum_            = (int64_t)0;
  point_data_.count_          = 0;
  point_data_.record_min_max_ = record_min_max_;
  point_data_.min_            = std::numeric_limits<int64_t>::max();
  point_data_.max_            = std::numeric_limits<int64_t>::min();
}

LongHistogramAggregation::LongHistogramAggregation(HistogramPointData &&data)
    : point_data_{std::move(data)}, record_min_max_{point_data_.record_min_max_}
{}

LongHistogramAggregation::LongHistogramAggregation(const HistogramPointData &data)
    : point_data_{data}, record_min_max_{point_data_.record_min_max_}
{}

void LongHistogramAggregation::Aggregate(int64_t value,
                                         const PointAttributes & /* attributes */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.count_ += 1;
  point_data_.sum_ = nostd::get<int64_t>(point_data_.sum_) + value;
  if (record_min_max_)
  {
    point_data_.min_ = std::min(nostd::get<int64_t>(point_data_.min_), value);
    point_data_.max_ = std::max(nostd::get<int64_t>(point_data_.max_), value);
  }
  size_t index = 0;
  for (auto it = point_data_.boundaries_.begin(); it != point_data_.boundaries_.end(); ++it)
  {
    if (value < *it)
    {
      point_data_.counts_[index] += 1;
      return;
    }
    index++;
  }
}

std::unique_ptr<Aggregation> LongHistogramAggregation::Merge(
    const Aggregation &delta) const noexcept
{
  auto curr_value  = nostd::get<HistogramPointData>(ToPoint());
  auto delta_value = nostd::get<HistogramPointData>(
      (static_cast<const LongHistogramAggregation &>(delta).ToPoint()));
  LongHistogramAggregation *aggr = new LongHistogramAggregation();
  HistogramMerge<int64_t>(curr_value, delta_value, aggr->point_data_);
  return std::unique_ptr<Aggregation>(aggr);
}

std::unique_ptr<Aggregation> LongHistogramAggregation::Diff(const Aggregation &next) const noexcept
{
  auto curr_value = nostd::get<HistogramPointData>(ToPoint());
  auto next_value = nostd::get<HistogramPointData>(
      (static_cast<const LongHistogramAggregation &>(next).ToPoint()));
  LongHistogramAggregation *aggr = new LongHistogramAggregation();
  HistogramDiff<int64_t>(curr_value, next_value, aggr->point_data_);
  return std::unique_ptr<Aggregation>(aggr);
}

PointType LongHistogramAggregation::ToPoint() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

DoubleHistogramAggregation::DoubleHistogramAggregation(const AggregationConfig *aggregation_config)
{
  auto ac = static_cast<const HistogramAggregationConfig *>(aggregation_config);
  if (ac && ac->boundaries_.size())
  {
    point_data_.boundaries_ = ac->boundaries_;
  }
  else
  {
    point_data_.boundaries_ =
        std::list<double>{0.0, 5.0, 10.0, 25.0, 50.0, 75.0, 100.0, 250.0, 500.0, 1000.0};
  }
  if (ac)
  {
    record_min_max_ = ac->record_min_max_;
  }
  point_data_.counts_         = std::vector<uint64_t>(point_data_.boundaries_.size() + 1, 0);
  point_data_.sum_            = 0.0;
  point_data_.count_          = 0;
  point_data_.record_min_max_ = record_min_max_;
  point_data_.min_            = std::numeric_limits<double>::max();
  point_data_.max_            = std::numeric_limits<double>::min();
}

DoubleHistogramAggregation::DoubleHistogramAggregation(HistogramPointData &&data)
    : point_data_{std::move(data)}
{}

DoubleHistogramAggregation::DoubleHistogramAggregation(const HistogramPointData &data)
    : point_data_{data}
{}

void DoubleHistogramAggregation::Aggregate(double value,
                                           const PointAttributes & /* attributes */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.count_ += 1;
  point_data_.sum_ = nostd::get<double>(point_data_.sum_) + value;
  if (record_min_max_)
  {
    point_data_.min_ = std::min(nostd::get<double>(point_data_.min_), value);
    point_data_.max_ = std::max(nostd::get<double>(point_data_.max_), value);
  }
  size_t index = 0;
  for (auto it = point_data_.boundaries_.begin(); it != point_data_.boundaries_.end(); ++it)
  {
    if (value < *it)
    {
      point_data_.counts_[index] += 1;
      return;
    }
    index++;
  }
}

std::unique_ptr<Aggregation> DoubleHistogramAggregation::Merge(
    const Aggregation &delta) const noexcept
{
  auto curr_value  = nostd::get<HistogramPointData>(ToPoint());
  auto delta_value = nostd::get<HistogramPointData>(
      (static_cast<const DoubleHistogramAggregation &>(delta).ToPoint()));
  std::shared_ptr<AggregationConfig> aggregation_config(new HistogramAggregationConfig);
  static_cast<opentelemetry::sdk::metrics::HistogramAggregationConfig *>(aggregation_config.get())
      ->boundaries_ = curr_value.boundaries_;
  static_cast<opentelemetry::sdk::metrics::HistogramAggregationConfig *>(aggregation_config.get())
      ->record_min_max_            = record_min_max_;
  DoubleHistogramAggregation *aggr = new DoubleHistogramAggregation(aggregation_config.get());
  HistogramMerge<double>(curr_value, delta_value, aggr->point_data_);
  return std::unique_ptr<Aggregation>(aggr);
}

std::unique_ptr<Aggregation> DoubleHistogramAggregation::Diff(
    const Aggregation &next) const noexcept
{
  auto curr_value = nostd::get<HistogramPointData>(ToPoint());
  auto next_value = nostd::get<HistogramPointData>(
      (static_cast<const DoubleHistogramAggregation &>(next).ToPoint()));
  DoubleHistogramAggregation *aggr = new DoubleHistogramAggregation();
  HistogramDiff<double>(curr_value, next_value, aggr->point_data_);
  return std::unique_ptr<Aggregation>(aggr);
}

PointType DoubleHistogramAggregation::ToPoint() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
