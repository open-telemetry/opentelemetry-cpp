// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/common/timestamp.h"
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
/**
 * This aggregator stores and maintains a vector of
 * type T where the contents of the vector simply
 * include the last value recorded to the aggregator.
 * The aggregator also maintains a timestamp of when
 * the last value was recorded.
 *
 * @tparam T the type of values stored in this aggregator.
 */
template <class T>
class GaugeAggregator : public Aggregator<T>
{
public:
  explicit GaugeAggregator(opentelemetry::metrics::InstrumentKind kind)
  {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
    this->kind_        = kind;
    this->values_      = std::vector<T>(1, 0);
    this->checkpoint_  = this->values_;
    this->agg_kind_    = AggregatorKind::Gauge;
    current_timestamp_ = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
  }

  ~GaugeAggregator() = default;

  GaugeAggregator(const GaugeAggregator &cp)
  {
    this->values_      = cp.values_;
    this->checkpoint_  = cp.checkpoint_;
    this->kind_        = cp.kind_;
    this->agg_kind_    = cp.agg_kind_;
    current_timestamp_ = cp.current_timestamp_;
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
    this->updated_     = true;
    this->values_[0]   = val;
    current_timestamp_ = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
    this->mu_.unlock();
  }

  /**
   * Checkpoints the current value.  This function will overwrite the current checkpoint with the
   * current value.
   *
   * @return none
   */

  void checkpoint() override
  {
    this->mu_.lock();

    this->updated_    = false;
    this->checkpoint_ = this->values_;

    // Reset the values to default
    this->values_[0]      = 0;
    checkpoint_timestamp_ = current_timestamp_;
    current_timestamp_ = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());

    this->mu_.unlock();
  }

  /**
   * Merges two Gauge aggregators together
   *
   * @param other the aggregator to merge with this aggregator
   */
  void merge(GaugeAggregator<T> other)
  {
    if (this->kind_ == other.kind_)
    {
      this->mu_.lock();
      // First merge values
      this->values_[0] = other.values_[0];
      // Now merge checkpoints
      this->checkpoint_[0] = other.checkpoint_[0];
      current_timestamp_ = opentelemetry::common::SystemTimestamp(std::chrono::system_clock::now());
      this->mu_.unlock();
    }
    else
    {
      // Log error
      return;
    }
  }

  /**
   * @return the value of the latest checkpoint
   */
  std::vector<T> get_checkpoint() override { return this->checkpoint_; }

  /**
   * @return the latest checkpointed timestamp
   */
  opentelemetry::common::SystemTimestamp get_checkpoint_timestamp() override
  {
    return checkpoint_timestamp_;
  }

  /**
   * @return the values_ vector stored in this aggregator
   */
  std::vector<T> get_values() override { return this->values_; }

  /**
   * @return the timestamp of when the last value recorded
   */
  opentelemetry::common::SystemTimestamp get_timestamp() { return current_timestamp_; }

private:
  opentelemetry::common::SystemTimestamp current_timestamp_;
  opentelemetry::common::SystemTimestamp checkpoint_timestamp_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
