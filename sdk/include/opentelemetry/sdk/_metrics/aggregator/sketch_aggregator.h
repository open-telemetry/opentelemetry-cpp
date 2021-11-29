// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <algorithm>
#  include <cmath>
#  include <limits>
#  include <map>
#  include <mutex>
#  include <stdexcept>
#  include <vector>
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/** Sketch Aggregators implement the DDSketch data type.  Note that data is compressed
 *  by the DDSketch algorithm and users should be informed about its behavior before
 *  selecting it as the aggregation type.  NOTE: The current implementation can only support
 *  non-negative values.
 *
 *  Detailed information about the algorithm can be found in the following paper
 *  published by Datadog: http://www.vldb.org/pvldb/vol12/p2195-masson.pdf
 */

template <class T>
class SketchAggregator final : public Aggregator<T>
{

public:
  /**
   * Given the distribution of data this aggregator is designed for and its usage, the raw updates
   *are stored in a map rather than a vector.
   *
   *@param kind, the instrument kind creating this aggregator
   *@param error_bound, what is referred to as "alpha" in the DDSketch algorithm
   *@param max_buckets, the maximum number of indices in the raw value map
   */
  SketchAggregator(opentelemetry::metrics::InstrumentKind kind,
                   double error_bound,
                   size_t max_buckets = 2048)
  {

    this->kind_       = kind;
    this->agg_kind_   = AggregatorKind::Sketch;
    this->values_     = std::vector<T>(2, 0);  // Sum in [0], Count in [1]
    this->checkpoint_ = std::vector<T>(2, 0);
    max_buckets_      = max_buckets;
    error_bound_      = error_bound;
    gamma             = (1 + error_bound) / (1 - error_bound);
  }

  /**
   * Update the aggregator with the new value.  For a DDSketch aggregator, if the addition of this
   * value creates a new bucket which is in excess of the maximum allowed size, the lowest indexes
   * buckets are merged.
   *
   * @param val, the raw value used in aggregation
   * @return none
   */
  void update(T val) override
  {
    this->mu_.lock();
    this->updated_ = true;
    int idx;
    if (val == 0)
    {
      idx = (std::numeric_limits<int>::min());
    }
    else
    {
      idx = static_cast<int>(ceil(log(val) / log(gamma)));
    }
    if (raw_.find(idx) != raw_.end())
    {
      raw_[idx] += 1;
    }
    else
    {
      raw_[idx] = 1;
    }
    this->values_[1] += 1;
    this->values_[0] += val;
    if (raw_.size() > max_buckets_)
    {
      int minidx = raw_.begin()->first, minidxval = raw_.begin()->second;
      raw_.erase(minidx);
      raw_[raw_.begin()->first] += minidxval;
    }
    this->mu_.unlock();
  }

  /**
   * Calculate and return the value of a user specified quantile.
   *
   * @param q, the quantile to calculate (for example 0.5 is equivalent to the 50th percentile)
   */
  virtual T get_quantiles(double q) override
  {
    if (q < 0 || q > 1)
    {
#  if __EXCEPTIONS
      throw std::invalid_argument("Quantile values must fall between 0 and 1");
#  else
      std::terminate();
#  endif
    }
    auto iter = checkpoint_raw_.begin();
    int idx   = iter->first;
    int count = iter->second;

    while (count < (q * (this->checkpoint_[1] - 1)) && iter != checkpoint_raw_.end())
    {
      iter++;
      idx = iter->first;
      count += iter->second;
    }
    return static_cast<T>(round(2 * pow(gamma, idx) / (gamma + 1)));
  }

  /**
   * Checkpoints the current value.  This function will overwrite the current checkpoint with the
   * current value.
   *
   * @param none
   * @return none
   */
  void checkpoint() override
  {
    this->mu_.lock();
    this->updated_    = false;
    this->checkpoint_ = this->values_;
    checkpoint_raw_   = raw_;
    this->values_[0]  = 0;
    this->values_[1]  = 0;
    raw_.clear();
    this->mu_.unlock();
  }

  /**
   * Merges this sketch aggregator with another.  The same bucket compression used when
   * updating values is employed here to manage bucket size if the merging of aggregators
   * results in more buckets than allowed.
   *
   * @param other, the aggregator with merge with
   * @return none
   */
  void merge(SketchAggregator other)
  {
    if (gamma != other.gamma)
    {
#  if __EXCEPTIONS
      throw std::invalid_argument("Aggregators must have identical error tolerance");
#  else
      std::terminate();
#  endif
    }
    else if (max_buckets_ != other.max_buckets_)
    {
#  if __EXCEPTIONS
      throw std::invalid_argument("Aggregators must have the same maximum bucket allowance");
#  else
      std::terminate();
#  endif
    }

    this->mu_.lock();
    this->values_[0] += other.values_[0];
    this->values_[1] += other.values_[1];
    this->checkpoint_[0] += other.checkpoint_[0];
    this->checkpoint_[1] += other.checkpoint_[1];
    auto other_iter = other.raw_.begin();
    while (other_iter != other.raw_.end())
    {
      raw_[other_iter->first] += other_iter->second;
      if (raw_.size() > max_buckets_)
      {
        int minidx = raw_.begin()->first, minidxval = raw_.begin()->second;
        raw_.erase(minidx);
        raw_[raw_.begin()->first] += minidxval;
      }
      other_iter++;
    }
    auto other_ckpt_iter = other.checkpoint_raw_.begin();
    while (other_ckpt_iter != other.checkpoint_raw_.end())
    {
      checkpoint_raw_[other_ckpt_iter->first] += other_ckpt_iter->second;
      if (checkpoint_raw_.size() > max_buckets_)
      {
        int minidx = checkpoint_raw_.begin()->first, minidxval = checkpoint_raw_.begin()->second;
        checkpoint_raw_.erase(minidx);
        checkpoint_raw_[checkpoint_raw_.begin()->first] += minidxval;
      }
      other_ckpt_iter++;
    }
    this->mu_.unlock();
  }

  /**
   * Returns the checkpointed value
   *
   * @param none
   * @return the value of the checkpoint
   */
  std::vector<T> get_checkpoint() override { return this->checkpoint_; }

  /**
   * Returns the current values
   *
   * @param none
   * @return the present aggregator values
   */
  std::vector<T> get_values() override { return this->values_; }

  /**
   * Returns the indices (or values) stored by this sketch aggregator.
   *
   * @param none
   * @return a vector of all values the aggregator is currently tracking
   */
  virtual std::vector<double> get_boundaries() override
  {
    std::vector<double> ret;
    for (auto const &x : checkpoint_raw_)
    {
      ret.push_back(2 * pow(gamma, x.first) / (gamma + 1));
    }
    return ret;
  }

  /**
   * Returns the error bound
   *
   * @param none
   * @return the error bound specified during construction
   */
  virtual double get_error_bound() override { return error_bound_; }

  /**
   * Returns the maximum allowed buckets
   *
   * @param none
   * @return the maximum allowed buckets
   */
  virtual size_t get_max_buckets() override { return max_buckets_; }

  /**
   * Returns the count of each value tracked by this sketch aggregator.  These are returned
   * in the same order as the indices returned by the get_boundaries function.
   *
   * @param none
   * @return a vector of all counts for values tracked by the aggregator
   */
  virtual std::vector<int> get_counts() override
  {
    std::vector<int> ret;
    for (auto const &x : checkpoint_raw_)
    {
      ret.push_back(x.second);
    }
    return ret;
  }

private:
  double gamma;
  double error_bound_;
  size_t max_buckets_;
  std::map<int, int> raw_;
  std::map<int, int> checkpoint_raw_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
