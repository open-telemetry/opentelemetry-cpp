// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"
#  include "opentelemetry/version.h"

#  include <memory>
#  include <mutex>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
const int MinValueIndex   = 0;
const int MaxValueIndex   = 1;
const int SumValueIndex   = 2;
const int CountValueIndex = 3;
/**
 * This aggregator stores and maintains a vector of
 * type T where the contents in the vector are made
 * up of the minimum value recorded to this instrument,
 * the maximum value, the sum of all values, and the
 * count of all values.
 *
 * @tparam T the type of values stored in this aggregator.
 */
template <class T>
class MinMaxSumCountAggregator : public Aggregator<T>
{
public:
  explicit MinMaxSumCountAggregator(opentelemetry::metrics::InstrumentKind kind)
  {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
    this->kind_       = kind;
    this->values_     = std::vector<T>(4, 0);  // {min, max, sum, count}
    this->checkpoint_ = this->values_;
    this->agg_kind_   = AggregatorKind::MinMaxSumCount;
  }

  ~MinMaxSumCountAggregator() = default;

  MinMaxSumCountAggregator(const MinMaxSumCountAggregator &cp)
  {
    this->values_     = cp.values_;
    this->checkpoint_ = cp.checkpoint_;
    this->kind_       = cp.kind_;
    this->agg_kind_   = cp.agg_kind_;
    // use default initialized mutex as they cannot be copied
  }

  /**
   * Receives a captured value from the instrument and applies it to the current aggregator value.
   *
   * @param val, the raw value used in aggregation
   */
  void update(T val) override
  {
    this->mu_.lock();
    this->updated_ = true;

    if (this->values_[CountValueIndex] == 0 || val < this->values_[MinValueIndex])  // set min
      this->values_[MinValueIndex] = val;
    if (this->values_[CountValueIndex] == 0 || val > this->values_[MaxValueIndex])  // set max
      this->values_[MaxValueIndex] = val;

    this->values_[SumValueIndex] += val;  // compute sum
    this->values_[CountValueIndex]++;     // increment count

    this->mu_.unlock();
  }

  /**
   * Checkpoints the current value.  This function will overwrite the current checkpoint with the
   * current value.
   *
   */
  void checkpoint() override
  {
    this->mu_.lock();
    this->updated_    = false;
    this->checkpoint_ = this->values_;
    // Reset the values
    this->values_[MinValueIndex]   = 0;
    this->values_[MaxValueIndex]   = 0;
    this->values_[SumValueIndex]   = 0;
    this->values_[CountValueIndex] = 0;
    this->mu_.unlock();
  }

  /**
   * Merges two MinMaxSumCount aggregators together
   *
   * @param other the aggregator to merge with this aggregator
   */
  void merge(const MinMaxSumCountAggregator &other)
  {
    if (this->kind_ == other.kind_)
    {
      this->mu_.lock();
      // First merge values
      // set min
      if (this->values_[CountValueIndex] == 0 ||
          other.values_[MinValueIndex] < this->values_[MinValueIndex])
        this->values_[MinValueIndex] = other.values_[MinValueIndex];
      // set max
      if (this->values_[CountValueIndex] == 0 ||
          other.values_[MaxValueIndex] > this->values_[MaxValueIndex])
        this->values_[MaxValueIndex] = other.values_[MaxValueIndex];
      // set sum
      this->values_[SumValueIndex] += other.values_[SumValueIndex];
      // set count
      this->values_[CountValueIndex] += other.values_[CountValueIndex];

      // Now merge checkpoints
      if (this->checkpoint_[CountValueIndex] == 0 ||
          other.checkpoint_[MinValueIndex] < this->checkpoint_[MinValueIndex])
        this->checkpoint_[MinValueIndex] = other.checkpoint_[MinValueIndex];
      // set max
      if (this->checkpoint_[CountValueIndex] == 0 ||
          other.checkpoint_[MaxValueIndex] > this->checkpoint_[MaxValueIndex])
        this->checkpoint_[MaxValueIndex] = other.checkpoint_[MaxValueIndex];
      // set sum
      this->checkpoint_[SumValueIndex] += other.checkpoint_[SumValueIndex];
      // set count
      this->checkpoint_[CountValueIndex] += other.checkpoint_[CountValueIndex];

      this->mu_.unlock();
    }
    else
    {
      // Log error
      return;
    }
  }

  /**
   * Returns the checkpointed value
   *
   * @return the value of the checkpoint
   */
  std::vector<T> get_checkpoint() override { return this->checkpoint_; }

  /**
   * Returns the values currently held by the aggregator
   *
   * @return the values held by the aggregator
   */
  std::vector<T> get_values() override { return this->values_; }
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
