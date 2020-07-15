#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"

#include <variant>
#include <vector>
#include <mutex>
#include <memory>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
template<class T>
class MinMaxSumCountAggregator : public Aggregator<T>
{
public:
  explicit MinMaxSumCountAggregator(metrics_api::BoundInstrumentKind kind)
  {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
    this->kind_ = kind;
    this->values_ = std::vector<T>(4, 0); // {min, max, sum count}
    this->checkpoint_ = this->values_;
  }

  /**
   * Receives a captured value from the instrument and applies it to the current aggregator value.
   *
   * @param val, the raw value used in aggregation
   */
  void update(T val) override
  {
    this->mu_.lock();

    if (this->values_[3] == 0 || val < this->values_[0]) // set min
      this->values_[0] = val;
    if (this->values_[3] == 0 || val > this->values_[1]) // set max
      this->values_[1] = val;

    this->values_[2] += val; // compute sum
    this->values_[3]++; // increment count

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
    this->checkpoint_ = this->values_;
    // Reset the values
    this->values_[0] = 0;
    this->values_[1] = 0;
    this->values_[2] = 0;
    this->values_[3] = 0;
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
      // set min
      if (other.values_[0] < this->values_[0])
        this->values_[0] = other.values_[0];
      // set max
      if (other.values_[1] > this->values_[1])
        this->values_[1] = other.values_[1];
      // set sum
      this->values_[2] += other.values_[2];
      // set count
      this->values_[3] += other.values_[3];
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
  std::vector<T> get_checkpoint() override
  {
    return this->checkpoint_;
  }

  /**
   * Returns the values currently held by the aggregator
   *
   * @return the values held by the aggregator
   */
  std::vector<T> get_values() override
  {
    return this->values_;
  }

};
}
}
OPENTELEMETRY_END_NAMESPACE
