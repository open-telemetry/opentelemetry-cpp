// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <iostream>
#  include <map>
#  include <memory>
#  include <sstream>
#  include <string>
#  include <unordered_map>
#  include <vector>
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/_metrics/record.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

#  if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4250)  // inheriting methods via dominance
#  endif

class Instrument : virtual public opentelemetry::metrics::Instrument
{

public:
  Instrument() = default;

  Instrument(nostd::string_view name,
             nostd::string_view description,
             nostd::string_view unit,
             bool enabled,
             opentelemetry::metrics::InstrumentKind kind)
      : name_(name), description_(description), unit_(unit), enabled_(enabled), kind_(kind)
  {}

  // Returns true if the instrument is enabled and collecting data
  virtual bool IsEnabled() override { return enabled_; }

  // Return the instrument name
  virtual nostd::string_view GetName() override { return name_; }

  // Return the instrument description
  virtual nostd::string_view GetDescription() override { return description_; }

  // Return the insrument's units of measurement
  virtual nostd::string_view GetUnits() override { return unit_; }

  virtual opentelemetry::metrics::InstrumentKind GetKind() override { return this->kind_; }

protected:
  std::string name_;
  std::string description_;
  std::string unit_;
  bool enabled_;
  std::mutex mu_;
  opentelemetry::metrics::InstrumentKind kind_;
};

template <class T>
class BoundSynchronousInstrument
    : public Instrument,
      virtual public opentelemetry::metrics::BoundSynchronousInstrument<T>
{

public:
  BoundSynchronousInstrument() = default;

  BoundSynchronousInstrument(nostd::string_view name,
                             nostd::string_view description,
                             nostd::string_view unit,
                             bool enabled,
                             opentelemetry::metrics::InstrumentKind kind,
                             std::shared_ptr<Aggregator<T>> agg)
      : Instrument(name, description, unit, enabled, kind), agg_(agg)
  {
    this->inc_ref();  // increase reference count when instantiated
  }

  /**
   * Frees the resources associated with this Bound Instrument.
   * The Metric from which this instrument was created is not impacted.
   *
   * @param none
   * @return void
   */
  virtual void unbind() override
  {
    this->mu_.lock();
    ref_ -= 1;
    this->mu_.unlock();
  }

  /**
   * Increments the reference count. This function is used when binding or instantiating.
   *
   * @param none
   * @return void
   */
  virtual void inc_ref() override
  {
    this->mu_.lock();
    ref_ += 1;
    this->mu_.unlock();
  }

  /**
   * Returns the current reference count of the instrument.  This value is used to
   * later in the pipeline remove stale instruments.
   *
   * @param none
   * @return current ref count of the instrument
   */
  virtual int get_ref() override
  {
    this->mu_.lock();
    auto ret = ref_;
    this->mu_.unlock();
    return ret;
  }

  /**
   * Records a single synchronous metric event via a call to the aggregator.
   * Since this is a bound synchronous instrument, labels are not required in
   * metric capture calls.
   *
   * @param value is the numerical representation of the metric being captured
   * @return void
   */
  virtual void update(T value) override
  {
    this->mu_.lock();
    agg_->update(value);
    this->mu_.unlock();
  }

  /**
   * Returns the aggregator responsible for meaningfully combining update values.
   *
   * @param none
   * @return the aggregator assigned to this instrument
   */
  virtual std::shared_ptr<Aggregator<T>> GetAggregator() final { return agg_; }

private:
  std::shared_ptr<Aggregator<T>> agg_;
  int ref_ = 0;
};

template <class T>
class SynchronousInstrument : public Instrument,
                              virtual public opentelemetry::metrics::SynchronousInstrument<T>
{

public:
  SynchronousInstrument() = default;

  SynchronousInstrument(nostd::string_view name,
                        nostd::string_view description,
                        nostd::string_view unit,
                        bool enabled,
                        opentelemetry::metrics::InstrumentKind kind)
      : Instrument(name, description, unit, enabled, kind)
  {}

  /**
   * Returns a Bound Instrument associated with the specified labels. Multiples requests
   * with the same set of labels may return the same Bound Instrument instance.
   *
   * It is recommended that callers keep a reference to the Bound Instrument
   * instead of repeatedly calling this operation.
   *
   * @param labels the set of labels, as key-value pairs
   * @return a Bound Instrument
   */
  virtual nostd::shared_ptr<opentelemetry::metrics::BoundSynchronousInstrument<T>> bind(
      const opentelemetry::common::KeyValueIterable &labels) override
  {
    return nostd::shared_ptr<BoundSynchronousInstrument<T>>();
  }

  // This function is necessary for batch recording and should NOT be called by the user
  virtual void update(T value, const opentelemetry::common::KeyValueIterable &labels) override = 0;

  /**
   * Checkpoints instruments and returns a set of records which are ready for processing.
   * This method should ONLY be called by the Meter Class as part of the export pipeline
   * as it also prunes bound instruments with no active references.
   *
   * @param none
   * @return vector of Records which hold the data attached to this synchronous instrument
   */
  virtual std::vector<Record> GetRecords() = 0;
};

template <class T>
class AsynchronousInstrument : public Instrument,
                               virtual public opentelemetry::metrics::AsynchronousInstrument<T>
{

public:
  AsynchronousInstrument() = default;

  AsynchronousInstrument(nostd::string_view name,
                         nostd::string_view description,
                         nostd::string_view unit,
                         bool enabled,
                         void (*callback)(opentelemetry::metrics::ObserverResult<T>),
                         opentelemetry::metrics::InstrumentKind kind)
      : Instrument(name, description, unit, enabled, kind)
  {
    this->callback_ = callback;
  }

  /**
   * Captures data through a manual call rather than the automatic collection process instituted
   * in the run function.  Asynchronous instruments are generally expected to obtain data from
   * their callbacks rather than direct calls.  This function is used by the callback to store data.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels is the numerical representation of the metric being captured
   * @return none
   */
  virtual void observe(T value, const opentelemetry::common::KeyValueIterable &labels) override = 0;

  virtual std::vector<Record> GetRecords() = 0;

  /**
   * Captures data by activating the callback function associated with the
   * instrument and storing its return value.  Callbacks for asynchronous
   * instruments are defined during construction.
   *
   * @param none
   * @return none
   */
  virtual void run() override = 0;
};

// Helper functions for turning a common::KeyValueIterable into a string
inline void print_value(std::stringstream &ss,
                        opentelemetry::common::AttributeValue &value,
                        bool jsonTypes = false)
{
  switch (value.index())
  {
    case opentelemetry::common::AttributeType::kTypeString:

      ss << nostd::get<nostd::string_view>(value);

      break;
    default:
#  if __EXCEPTIONS
      throw std::invalid_argument("Labels must be strings");
#  else
      std::terminate();
#  endif
      break;
  }
};

// Utility function which converts maps to strings for better performance
inline std::string mapToString(const std::map<std::string, std::string> &conv)
{
  std::stringstream ss;
  ss << "{";
  for (auto i : conv)
  {
    ss << i.first << ':' << i.second << ',';
  }
  ss << "}";
  return ss.str();
}

inline std::string KvToString(const opentelemetry::common::KeyValueIterable &kv) noexcept
{
  std::stringstream ss;
  ss << "{";
  size_t size = kv.size();
  if (size)
  {
    size_t i = 1;
    kv.ForEachKeyValue(
        [&](nostd::string_view key, opentelemetry::common::AttributeValue value) noexcept {
          ss << key << ":";
          print_value(ss, value, true);
          if (size != i)
          {
            ss << ",";
          }
          i++;
          return true;
        });
  };
  ss << "}";
  return ss.str();
}

#  if defined(_MSC_VER)
#    pragma warning(pop)
#  endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
