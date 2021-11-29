// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <mutex>
#  include <vector>
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

enum class AggregatorKind
{
  Counter        = 0,
  MinMaxSumCount = 1,
  Gauge          = 2,
  Sketch         = 3,
  Histogram      = 4,
  Exact          = 5,
};

/*
 * Performs calculations necessary to combine updates from instruments into an
 * insightful value.
 * Also stores current instrument values and checkpoints collected at intervals
 * governing the entire pipeline.
 */
template <typename T>
class Aggregator
{
public:
  Aggregator() = default;

  virtual ~Aggregator() = default;

  /**
   * Receives a captured value from the instrument and applies it to the current aggregator value.
   *
   * @param val, the raw value used in aggregation
   * @return none
   */
  virtual void update(T val) = 0;

  /**
   * Checkpoints the current value.  This function will overwrite the current checkpoint with the
   * current value.
   *
   * @param none
   * @return none
   */
  virtual void checkpoint() = 0;

  /**
   * Merges the values of two aggregators in a semantically accurate manner.
   * Merging will occur differently for different aggregators depending on the
   * way values are tracked.
   *
   * @param other, the aggregator with merge with
   * @return none
   */
  void merge(Aggregator *other);

  /**
   * Returns the checkpointed value
   *
   * @param none
   * @return the value of the checkpoint
   */
  virtual std::vector<T> get_checkpoint() = 0;

  /**
   * Returns the current value
   *
   * @param none
   * @return the present aggregator value
   */
  virtual std::vector<T> get_values() = 0;

  /**
   * Returns the instrument kind which this aggregator is associated with
   *
   * @param none
   * @return the InstrumentKind of the aggregator's owner
   */
  virtual opentelemetry::metrics::InstrumentKind get_instrument_kind() final { return kind_; }

  /**
   * Returns the type of this aggregator
   *
   * @param none
   * @return the AggregatorKind of this instrument
   */
  virtual AggregatorKind get_aggregator_kind() final { return agg_kind_; }

  /**
   * Getter function for updated_ protected var
   *
   * @return A bool indicating wether or not this aggregator has been updated
   * in the most recent collection interval.
   */
  virtual bool is_updated() final { return updated_; }

  // virtual function to be overridden for the Histogram Aggregator
  virtual std::vector<double> get_boundaries() { return std::vector<double>(); }

  // virtual function to be overridden for the Histogram Aggregator
  virtual std::vector<int> get_counts() { return std::vector<int>(); }

  // virtual function to be overridden for Exact and Sketch Aggregators
  virtual bool get_quant_estimation() { return false; }

  // virtual function to be overridden for Exact and Sketch Aggregators
  virtual T get_quantiles(double q) { return values_[0]; }

  // virtual function to be overridden for Sketch Aggregator
  virtual double get_error_bound() { return 0; }

  // virtual function to be overridden for Sketch Aggregator
  virtual size_t get_max_buckets() { return 0; }

  // virtual function to be overridden for Gauge Aggregator
  virtual opentelemetry::common::SystemTimestamp get_checkpoint_timestamp()
  {
    return opentelemetry::common::SystemTimestamp();
  }

  // Custom copy constructor to handle the mutex
  Aggregator(const Aggregator &cp)
  {
    values_     = cp.values_;
    checkpoint_ = cp.checkpoint_;
    kind_       = cp.kind_;
    agg_kind_   = cp.agg_kind_;
    // use default initialized mutex as they cannot be copied
  }

protected:
  std::vector<T> values_;
  std::vector<T> checkpoint_;
  opentelemetry::metrics::InstrumentKind kind_;
  std::mutex mu_;
  AggregatorKind agg_kind_;
  bool updated_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
