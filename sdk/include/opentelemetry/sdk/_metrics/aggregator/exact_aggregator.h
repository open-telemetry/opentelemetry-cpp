// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"
#  include "opentelemetry/version.h"

#  include <cmath>
#  include <memory>
#  include <mutex>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * This aggregator has two modes. In-order and quantile estimation.
 *
 * The first mode simply stores all values sent to the Update()
 * function in a vector and maintains the order they were sent in.
 *
 * The second mode also stores all values sent to the Update()
 * function in a vector but sorts this vector when Checkpoint()
 * is called. This mode also includes a function, Quantile(),
 * that estimates the quantiles of the recorded data.
 *
 * @tparam T the type of values stored in this aggregator.
 */
template <class T>
class ExactAggregator : public Aggregator<T>
{
public:
  ExactAggregator(opentelemetry::metrics::InstrumentKind kind, bool quant_estimation = false)
  {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
    this->kind_       = kind;
    this->checkpoint_ = this->values_;
    this->agg_kind_   = AggregatorKind::Exact;
    quant_estimation_ = quant_estimation;
  }

  ~ExactAggregator() = default;

  ExactAggregator(const ExactAggregator &cp)
  {
    this->values_     = cp.values_;
    this->checkpoint_ = cp.checkpoint_;
    this->kind_       = cp.kind_;
    this->agg_kind_   = cp.agg_kind_;
    quant_estimation_ = cp.quant_estimation_;
    // use default initialized mutex as they cannot be copied
  }

  /**
   * Receives a captured value from the instrument and adds it to the values_ vector.
   *
   * @param val, the raw value used in aggregation
   */
  void update(T val) override
  {
    this->mu_.lock();
    this->updated_ = true;
    this->values_.push_back(val);
    this->mu_.unlock();
  }

  /**
   * Checkpoints the current values.  This function will overwrite the current checkpoint with the
   * current value. Sorts the values_ vector if quant_estimation_ == true
   *
   */
  void checkpoint() override
  {
    this->mu_.lock();
    this->updated_ = false;
    if (quant_estimation_)
    {
      std::sort(this->values_.begin(), this->values_.end());
    }
    this->checkpoint_ = this->values_;
    this->values_.clear();
    this->mu_.unlock();
  }

  /**
   * Merges two exact aggregators' values_ vectors together.
   *
   * @param other the aggregator to merge with this aggregator
   */
  void merge(const ExactAggregator &other)
  {
    if (this->kind_ == other.kind_)
    {
      this->mu_.lock();
      // First merge values
      this->values_.insert(this->values_.end(), other.values_.begin(), other.values_.end());
      // Now merge checkpoints
      this->checkpoint_.insert(this->checkpoint_.end(), other.checkpoint_.begin(),
                               other.checkpoint_.end());
      this->mu_.unlock();
    }
    else
    {
      // Log error
      return;
    }
  }

  /**
   * Performs quantile estimation on the checkpoint vector in this aggregator.
   * This function only works if quant_estimation_ == true.
   * @param q the quantile to estimate. 0 <= q <= 1
   * @return the nearest value in the vector to the exact quantile.
   */
  T get_quantiles(double q) override
  {
    if (!quant_estimation_)
    {
// Log error
#  if __EXCEPTIONS
      throw std::domain_error("Exact aggregator is not in quantile estimation mode!");
#  else
      std::terminate();
#  endif
    }
    if (this->checkpoint_.size() == 0 || q < 0 || q > 1)
    {
// Log error
#  if __EXCEPTIONS
      throw std::invalid_argument("Arg 'q' must be between 0 and 1, inclusive");
#  else
      std::terminate();
#  endif
    }
    else if (q == 0 || this->checkpoint_.size() == 1)
    {
      return this->checkpoint_[0];
    }
    else if (q == 1)
    {
      return this->checkpoint_[this->checkpoint_.size() - 1];
    }
    else
    {
      float position = float(float(this->checkpoint_.size() - 1) * q);
      int ceiling    = int(ceil(position));
      return this->checkpoint_[ceiling];
    }
  }

  //////////////////////////ACCESSOR FUNCTIONS//////////////////////////
  std::vector<T> get_checkpoint() override { return this->checkpoint_; }

  std::vector<T> get_values() override { return this->values_; }

  bool get_quant_estimation() override { return quant_estimation_; }

private:
  bool quant_estimation_;  // Used to switch between in-order and quantile estimation modes
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
