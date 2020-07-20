#pragma once

#include "instrument.h"
#include "observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

template <class T>
class ValueObserver : public AsynchronousInstrument<T>
{

public:
  ValueObserver() = default;

  ValueObserver(nostd::string_view name,
                   nostd::string_view description,
                   nostd::string_view unit,
                   bool enabled,
                   void (*callback)(ObserverResult<T>))
  {}

  /*
   * Updates the instruments aggregator with the new value. The labels should
   * contain the keys and values to be associated with this value.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(T value, const trace::KeyValueIterable &labels) {}
};

template <class T>
class SumObserver : public AsynchronousInstrument<T>
{

public:
  SumObserver() = default;

  SumObserver(nostd::string_view name,
                 nostd::string_view description,
                 nostd::string_view unit,
                 bool enabled,
                 void (*callback)(ObserverResult<T>))
  {}

  /*
   * Add adds the value to the counter's sum. The labels should contain
   * the keys and values to be associated with this value.  Counters only     * accept positive
   * valued updates.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(T value, const trace::KeyValueIterable &labels) {}
};

template <class T>
class UpDownSumObserver : public AsynchronousInstrument<T>
{

public:
  UpDownSumObserver() = default;

  UpDownSumObserver(nostd::string_view name,
                       nostd::string_view description,
                       nostd::string_view unit,
                       bool enabled,
                       void (*callback)(ObserverResult<T>))
  {}

  virtual void observe(int value, const trace::KeyValueIterable &labels) {}
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
