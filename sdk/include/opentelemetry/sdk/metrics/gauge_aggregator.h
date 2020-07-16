#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#include "opentelemetry/core/timestamp.h"

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
/**
 * This aggregator stores and maintains a vector of
 * type T where the contents of the vector simply
 * include the last value recorded to the aggregator.
 * The aggregator also maintains a timestamp of when
 * the last value was recorded.
 *
 * @tparam T the type of values stored in this aggregator.
 */
template<class T>
class GaugeAggregator : public Aggregator<T>
{
public:
  explicit GaugeAggregator<T>(metrics_api::BoundInstrumentKind kind)
  {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
    this->kind_ = kind;
    this->values_ = std::vector<T>(1, 0);
    this->checkpoint_ = this->values_;
    cur_timestamp = core::SystemTimestamp(std::chrono::system_clock::now());
  }

  /**
   * Receives a captured value from the instrument and applies it to the current aggregator value.
   *
   * @param val, the raw value used in aggregation
   */
  void update(T val) override
  {
    this->mu_.lock();
    this->values_[0] = val;
    cur_timestamp = core::SystemTimestamp(std::chrono::system_clock::now());
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

    this->checkpoint_ = this->values_;

    // Reset the values to default
    this->values_[0] = 0;
    checkpoint_timestamp = cur_timestamp;
    cur_timestamp = core::SystemTimestamp(std::chrono::system_clock::now());

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
      this->values_[0] = other.values_[0];
      cur_timestamp = core::SystemTimestamp(std::chrono::system_clock::now());
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
  std::vector<T> get_checkpoint() override
  {
    return this->checkpoint_;
  }

  /**
   * @return the latest checkpointed timestamp
   */
  core::SystemTimestamp get_checkpoint_timestamp()
  {
    return checkpoint_timestamp;
  }

  /**
   * @return the values_ vector stored in this aggregator
   */
  std::vector<T> get_values() override
  {
    return this->values_;
  }

  /**
   * @return the timestamp of when the last value recorded
   */
  core::SystemTimestamp get_timestamp()
  {
    return cur_timestamp;
  }

private:
  core::SystemTimestamp cur_timestamp;
  core::SystemTimestamp checkpoint_timestamp;
};
}
}
OPENTELEMETRY_END_NAMESPACE
