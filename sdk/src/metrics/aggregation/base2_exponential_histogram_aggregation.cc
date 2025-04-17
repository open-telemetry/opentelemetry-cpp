// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/aggregation/base2_exponential_histogram_aggregation.h"
#include <stddef.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <utility>
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/aggregation/base2_exponential_histogram_indexer.h"
#include "opentelemetry/sdk/metrics/data/circular_buffer.h"
#include "opentelemetry/sdk/metrics/data/metric_data.h"
#include "opentelemetry/sdk/metrics/data/point_data.h"
#include "opentelemetry/version.h"

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

// Commented out as it is not used
// uint32_t GetScaleReduction(const AdaptingCircularBufferCounter &first,
//                            const AdaptingCircularBufferCounter &second,
//                            size_t max_buckets)
// {
//   if (first.Empty() || second.Empty())
//   {
//     return 0;
//   }
//   const int32_t start_index = (std::min)(first.StartIndex(), second.StartIndex());
//   const int32_t end_index   = (std::max)(first.EndIndex(), second.EndIndex());
//   return GetScaleReduction(start_index, end_index, max_buckets);
// }

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
      new_buckets.Increment(i >> by, count);
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
  point_data_.min_            = (std::numeric_limits<double>::max)();
  point_data_.max_            = (std::numeric_limits<double>::min)();

  indexer_ = Base2ExponentialHistogramIndexer(point_data_.scale_);
}

Base2ExponentialHistogramAggregation::Base2ExponentialHistogramAggregation(
    const Base2ExponentialHistogramPointData &point_data)
    : point_data_{point_data},
      indexer_(point_data.scale_),
      record_min_max_{point_data.record_min_max_}
{}

Base2ExponentialHistogramAggregation::Base2ExponentialHistogramAggregation(
    Base2ExponentialHistogramPointData &&point_data)
    : point_data_{std::move(point_data)},
      indexer_(point_data_.scale_),
      record_min_max_{point_data_.record_min_max_}
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
  point_data_.count_++;

  if (record_min_max_)
  {
    point_data_.min_ = (std::min)(point_data_.min_, value);
    point_data_.max_ = (std::max)(point_data_.max_, value);
  }

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
    const int32_t start_index = (std::min)(buckets->StartIndex(), index);
    const int32_t end_index   = (std::max)(buckets->EndIndex(), index);
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

// Merge A and B into a new circular buffer C.
// Caller must ensure that A and B are used as buckets at the same scale.
AdaptingCircularBufferCounter MergeBuckets(size_t max_buckets,
                                           const AdaptingCircularBufferCounter &A,
                                           const AdaptingCircularBufferCounter &B)
{
  AdaptingCircularBufferCounter C = AdaptingCircularBufferCounter(max_buckets);
  C.Clear();

  if (A.Empty() && B.Empty())
  {
    return C;
  }
  if (A.Empty())
  {
    return B;
  }
  if (B.Empty())
  {
    return A;
  }

  auto min_index = (std::min)(A.StartIndex(), B.StartIndex());
  auto max_index = (std::max)(A.EndIndex(), B.EndIndex());

  for (int i = min_index; i <= max_index; i++)
  {
    auto count = A.Get(i) + B.Get(i);
    if (count > 0)
    {
      C.Increment(i, count);
    }
  }

  return C;
}

std::unique_ptr<Aggregation> Base2ExponentialHistogramAggregation::Merge(
    const Aggregation &delta) const noexcept
{
  auto left  = nostd::get<Base2ExponentialHistogramPointData>(ToPoint());
  auto right = nostd::get<Base2ExponentialHistogramPointData>(
      (static_cast<const Base2ExponentialHistogramAggregation &>(delta).ToPoint()));

  auto low_res         = left.scale_ < right.scale_ ? left : right;
  auto high_res        = left.scale_ < right.scale_ ? right : left;
  auto scale_reduction = high_res.scale_ - low_res.scale_;

  Base2ExponentialHistogramPointData result_value;
  result_value.count_      = low_res.count_ + high_res.count_;
  result_value.sum_        = low_res.sum_ + high_res.sum_;
  result_value.zero_count_ = low_res.zero_count_ + high_res.zero_count_;
  result_value.scale_      = (std::min)(low_res.scale_, high_res.scale_);
  result_value.max_buckets_ =
      low_res.max_buckets_ >= high_res.max_buckets_ ? low_res.max_buckets_ : high_res.max_buckets_;
  result_value.record_min_max_ = low_res.record_min_max_ && high_res.record_min_max_;
  if (result_value.record_min_max_)
  {
    result_value.min_ = (std::min)(low_res.min_, high_res.min_);
    result_value.max_ = (std::max)(low_res.max_, high_res.max_);
  }

  if (scale_reduction > 0)
  {
    DownscaleBuckets(&high_res.positive_buckets_, scale_reduction);
    DownscaleBuckets(&high_res.negative_buckets_, scale_reduction);
    high_res.scale_ -= scale_reduction;
  }

  auto pos_min_index =
      (std::min)(low_res.positive_buckets_.StartIndex(), high_res.positive_buckets_.StartIndex());
  auto pos_max_index =
      (std::max)(low_res.positive_buckets_.EndIndex(), high_res.positive_buckets_.EndIndex());
  auto neg_min_index =
      (std::min)(low_res.negative_buckets_.StartIndex(), high_res.negative_buckets_.StartIndex());
  auto neg_max_index =
      (std::max)(low_res.negative_buckets_.EndIndex(), high_res.negative_buckets_.EndIndex());

  if (pos_max_index > pos_min_index + result_value.max_buckets_ ||
      neg_max_index > neg_min_index + result_value.max_buckets_)
  {
    // We need to downscale the buckets to fit into the new max_buckets_.
    const uint32_t scale_reduction =
        GetScaleReduction(pos_min_index, pos_max_index, result_value.max_buckets_);
    DownscaleBuckets(&low_res.positive_buckets_, scale_reduction);
    DownscaleBuckets(&high_res.positive_buckets_, scale_reduction);
    DownscaleBuckets(&low_res.negative_buckets_, scale_reduction);
    DownscaleBuckets(&high_res.negative_buckets_, scale_reduction);
    low_res.scale_ -= scale_reduction;
    high_res.scale_ -= scale_reduction;
    result_value.scale_ -= scale_reduction;
  }

  result_value.positive_buckets_ = MergeBuckets(
      result_value.max_buckets_, low_res.positive_buckets_, high_res.positive_buckets_);
  result_value.negative_buckets_ = MergeBuckets(
      result_value.max_buckets_, low_res.negative_buckets_, high_res.negative_buckets_);

  return std::unique_ptr<Base2ExponentialHistogramAggregation>{
      new Base2ExponentialHistogramAggregation(std::move(result_value))};
}

std::unique_ptr<Aggregation> Base2ExponentialHistogramAggregation::Diff(
    const Aggregation &next) const noexcept
{
  auto left  = nostd::get<Base2ExponentialHistogramPointData>(ToPoint());
  auto right = nostd::get<Base2ExponentialHistogramPointData>(
      (static_cast<const Base2ExponentialHistogramAggregation &>(next).ToPoint()));

  auto low_res         = left.scale_ < right.scale_ ? left : right;
  auto high_res        = left.scale_ < right.scale_ ? right : left;
  auto scale_reduction = high_res.scale_ - low_res.scale_;

  if (scale_reduction > 0)
  {
    DownscaleBuckets(&high_res.positive_buckets_, scale_reduction);
    DownscaleBuckets(&high_res.negative_buckets_, scale_reduction);
    high_res.scale_ -= scale_reduction;
  }

  auto pos_min_index =
      (std::min)(left.positive_buckets_.StartIndex(), right.positive_buckets_.StartIndex());
  auto pos_max_index =
      (std::max)(left.positive_buckets_.EndIndex(), right.positive_buckets_.EndIndex());
  auto neg_min_index =
      (std::min)(left.negative_buckets_.StartIndex(), right.negative_buckets_.StartIndex());
  auto neg_max_index =
      (std::max)(left.negative_buckets_.EndIndex(), right.negative_buckets_.EndIndex());

  if (pos_max_index > pos_min_index + low_res.max_buckets_ ||
      neg_max_index > neg_min_index + low_res.max_buckets_)
  {
    // We need to downscale the buckets to fit into the new max_buckets_.
    const uint32_t scale_reduction =
        GetScaleReduction(pos_min_index, pos_max_index, low_res.max_buckets_);
    DownscaleBuckets(&left.positive_buckets_, scale_reduction);
    DownscaleBuckets(&right.positive_buckets_, scale_reduction);
    DownscaleBuckets(&left.negative_buckets_, scale_reduction);
    DownscaleBuckets(&right.negative_buckets_, scale_reduction);
    left.scale_ -= scale_reduction;
    right.scale_ -= scale_reduction;
  }

  Base2ExponentialHistogramPointData result_value;
  result_value.scale_          = low_res.scale_;
  result_value.max_buckets_    = low_res.max_buckets_;
  result_value.record_min_max_ = false;
  // caution for underflow
  // expect right.{sum, count} >= left.{sum, count} since metric points should be monotonically
  // increasing
  result_value.count_ = (right.count_ >= left.count_) ? (right.count_ - left.count_) : 0.0;
  result_value.sum_   = (right.sum_ >= left.sum_) ? (right.sum_ - left.sum_) : 0.0;
  result_value.zero_count_ =
      (right.zero_count_ >= left.zero_count_) ? (right.zero_count_ - left.zero_count_) : 0;
  result_value.positive_buckets_ = AdaptingCircularBufferCounter{right.max_buckets_};
  result_value.negative_buckets_ = AdaptingCircularBufferCounter{right.max_buckets_};

  if (!left.positive_buckets_.Empty() && !right.positive_buckets_.Empty())
  {
    for (auto i = pos_min_index; i <= pos_max_index; i++)
    {
      auto l_cnt = left.positive_buckets_.Get(i);
      auto r_cnt = right.positive_buckets_.Get(i);
      // expect right >= left since metric points should be monotonically increasing
      auto delta = (std::max)((uint64_t)(0), r_cnt - l_cnt);
      if (l_cnt > 0)
      {
        result_value.positive_buckets_.Increment(i, delta);
      }
    }
  }

  if (!left.negative_buckets_.Empty() && !right.negative_buckets_.Empty())
  {
    for (auto i = neg_min_index; i <= neg_max_index; i++)
    {
      auto l_cnt = left.negative_buckets_.Get(i);
      auto r_cnt = right.negative_buckets_.Get(i);
      // expect right >= left since metric points should be monotonically increasing
      auto delta = (std::max)((uint64_t)(0), r_cnt - l_cnt);
      if (delta > 0)
      {
        result_value.negative_buckets_.Increment(i, delta);
      }
    }
  }

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
