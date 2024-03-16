// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/aggregation/base2_exponential_histogram_aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/data/circular_buffer.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/version.h"

#include <cmath>
#include <cstddef>
#include <exception>
#include <limits>
#include <memory>
#include <mutex>

#include <iostream>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace
{

uint32_t GetScaleReduction(int32_t start_index, int32_t end_index, size_t max_buckets) noexcept
{
  uint32_t scale_reduction = 0;
  while (static_cast<size_t>(end_index - start_index + 1) > max_buckets)
  {
    start_index >>= 1;
    end_index >>= 1;
    scale_reduction++;
  }
  return scale_reduction;
}

uint32_t GetScaleReduction(const AdaptingCircularBufferCounter &first,
                           const AdaptingCircularBufferCounter &second,
                           size_t max_buckets)
{
  if (first.Empty() || second.Empty())
  {
    return 0;
  }

  const int32_t start_index = std::min(first.StartIndex(), second.StartIndex());
  const int32_t end_index   = std::max(first.EndIndex(), second.EndIndex());
  return GetScaleReduction(start_index, end_index, max_buckets);
}

void DownscaleBuckets(AdaptingCircularBufferCounter *buckets, uint32_t by) noexcept
{
  if (buckets->Empty())
  {
    return;
  }

  // We want to preserve other optimisations here as well, e.g. integer size.
  // Instead of  creating a new counter, we copy the existing one (for bucket size
  // optimisations), and clear the values before writing the new ones.
  // TODO(euroelessar): Do downscaling in-place.
  AdaptingCircularBufferCounter new_buckets = *buckets;
  new_buckets.Clear();

  for (int i = buckets->StartIndex(); i <= buckets->EndIndex(); i++)
  {
    const uint64_t count = buckets->Get(i);
    if (count > 0)
    {
      buckets->Increment(i >> by, count);
    }
  }
  *buckets = std::move(new_buckets);
}

}  // namespace

Base2ExponentialHistogramAggregation::Base2ExponentialHistogramAggregation(
    const AggregationConfig *aggregation_config)
{
  const Base2ExponentialHistogramAggregationConfig default_config;
  auto ac = static_cast<const Base2ExponentialHistogramAggregationConfig *>(aggregation_config);
  if (!ac)
  {
    ac = &default_config;
  }

  point_data_.max_buckets_    = ac->max_buckets_;
  point_data_.scale_          = ac->max_scale_;
  point_data_.record_min_max_ = ac->record_min_max_;
  point_data_.min_            = std::numeric_limits<double>::max();
  point_data_.max_            = std::numeric_limits<double>::min();

  indexer_ = Base2ExponentialHistogramIndexer(point_data_.scale_);
}

Base2ExponentialHistogramAggregation::Base2ExponentialHistogramAggregation(
    Base2ExponentialHistogramPointData point_data)
    : point_data_{std::move(point_data)}, indexer_(point_data.scale_)
{}

void Base2ExponentialHistogramAggregation::Aggregate(
    int64_t value,
    const PointAttributes & /* attributes */) noexcept
{
  Aggregate(double(value));
}

void Base2ExponentialHistogramAggregation::Aggregate(
    double value,
    const PointAttributes & /* attributes */) noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  point_data_.sum_ += value;
  point_data_.min_ = std::min(point_data_.min_, value);
  point_data_.max_ = std::max(point_data_.max_, value);
  point_data_.count_++;

  if (value == 0)
  {
    point_data_.zero_count_++;
    return;
  }
  else if (value > 0)
  {
    AggregateIntoBuckets(&point_data_.positive_buckets_, value);
  }
  else
  {
    AggregateIntoBuckets(&point_data_.negative_buckets_, -value);
  }
}

void Base2ExponentialHistogramAggregation::AggregateIntoBuckets(
    AdaptingCircularBufferCounter *buckets,
    double value) noexcept
{
  if (buckets->MaxSize() == 0)
  {
    *buckets = AdaptingCircularBufferCounter{point_data_.max_buckets_};
  }

  const int32_t index = indexer_.ComputeIndex(value);
  if (!buckets->Increment(index, 1))
  {
    const int32_t start_index = std::min(buckets->StartIndex(), index);
    const int32_t end_index   = std::max(buckets->EndIndex(), index);
    const uint32_t scale_reduction =
        GetScaleReduction(start_index, end_index, point_data_.max_buckets_);
    Downscale(scale_reduction);

    buckets->Increment(index >> scale_reduction, 1);
  }
}

void Base2ExponentialHistogramAggregation::Downscale(uint32_t by) noexcept
{
  if (by == 0)
  {
    return;
  }

  DownscaleBuckets(&point_data_.positive_buckets_, by);
  DownscaleBuckets(&point_data_.negative_buckets_, by);

  point_data_.scale_ -= by;
  indexer_ = Base2ExponentialHistogramIndexer(point_data_.scale_);
}

std::unique_ptr<Aggregation> Base2ExponentialHistogramAggregation::Merge(
    const Aggregation &delta) const noexcept
{
  auto curr_value  = nostd::get<Base2ExponentialHistogramPointData>(ToPoint());
  auto delta_value = nostd::get<Base2ExponentialHistogramPointData>(
      (static_cast<const Base2ExponentialHistogramAggregation &>(delta).ToPoint()));
  Base2ExponentialHistogramAggregationConfig agg_config;
  agg_config.max_scale_      = std::min(curr_value.scale_, delta_value.scale_);
  agg_config.max_buckets_    = curr_value.max_buckets_;
  agg_config.record_min_max_ = curr_value.record_min_max_ && delta_value.record_min_max_;

  Base2ExponentialHistogramPointData result_value;
  result_value.count_            = curr_value.count_ + delta_value.count_;
  result_value.sum_              = curr_value.sum_ + delta_value.sum_;
  result_value.zero_count_       = curr_value.zero_count_ + delta_value.zero_count_;
  result_value.min_              = std::min(curr_value.min_, delta_value.min_);
  result_value.max_              = std::max(curr_value.max_, delta_value.max_);
  result_value.positive_buckets_ = std::move(curr_value.positive_buckets_);
  result_value.record_min_max_   = curr_value.record_min_max_ && delta_value.record_min_max_;
  // if (!delta_value.positive_buckets_.Empty())
  // {
  // }
  result_value.negative_buckets_ = std::move(curr_value.negative_buckets_);
  // if (!delta_value.negative_buckets_.Empty())
  // {
  //   for (int i = delta_value.negative_buckets_.StartIndex();
  //        i <= delta_value.negative_buckets_.EndIndex(); i++)
  //   {
  //     result.negative_buckets_.Increment(i, delta);
  //   }
  // }

  return std::unique_ptr<Base2ExponentialHistogramAggregation>{
      new Base2ExponentialHistogramAggregation(std::move(result_value))};
}

std::unique_ptr<Aggregation> Base2ExponentialHistogramAggregation::Diff(
    const Aggregation &next) const noexcept
{
  auto curr_value = nostd::get<Base2ExponentialHistogramPointData>(ToPoint());
  auto next_value = nostd::get<Base2ExponentialHistogramPointData>(
      (static_cast<const Base2ExponentialHistogramAggregation &>(next).ToPoint()));

  Base2ExponentialHistogramPointData result_value;
  result_value.scale_          = curr_value.scale_;
  result_value.max_buckets_    = curr_value.max_buckets_;
  result_value.record_min_max_ = false;
  result_value.count_          = next_value.count_ - curr_value.count_;
  result_value.sum_            = next_value.sum_ - curr_value.sum_;
  result_value.zero_count_     = next_value.zero_count_ - curr_value.zero_count_;

  return std::unique_ptr<Base2ExponentialHistogramAggregation>{
      new Base2ExponentialHistogramAggregation(std::move(result_value))};
}

PointType Base2ExponentialHistogramAggregation::ToPoint() const noexcept
{
  const std::lock_guard<opentelemetry::common::SpinLockMutex> locked(lock_);
  return point_data_;
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
