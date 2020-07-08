#pragma once

#include "instrument.h"
#include "observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

class IntValueObserver : public AsynchronousInstrument
{

public:
  IntValueObserver() = default;

  IntValueObserver(nostd::string_view name,
                   nostd::string_view description,
                   nostd::string_view unit,
                   bool enabled,
                   void (*callback)(ObserverResult))
  {}

  /*
   * Updates the instruments aggregator with the new value. The labels should
   * contain the keys and values to be associated with this value.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(int value, const trace::KeyValueIterable &labels) {}
};

class DoubleValueObserver : public AsynchronousInstrument
{

public:
  DoubleValueObserver() = default;

  DoubleValueObserver(nostd::string_view name,
                      nostd::string_view description,
                      nostd::string_view unit,
                      bool enabled,
                      void (*callback)(ObserverResult))
  {}

  virtual void observe(double value, const trace::KeyValueIterable &labels) {}
};

class IntSumObserver : public AsynchronousInstrument
{

public:
  IntSumObserver() = default;

  IntSumObserver(nostd::string_view name,
                 nostd::string_view description,
                 nostd::string_view unit,
                 bool enabled,
                 void (*callback)(ObserverResult))
  {}

  /*
   * Add adds the value to the counter's sum. The labels should contain
   * the keys and values to be associated with this value.  Counters only     * accept positive
   * valued updates.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(int value, const trace::KeyValueIterable &labels) {}
};

class DoubleSumObserver : public AsynchronousInstrument
{

public:
  DoubleSumObserver() = default;

  DoubleSumObserver(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit,
                    bool enabled,
                    void (*callback)(ObserverResult))
  {}

  virtual void observe(double value, const trace::KeyValueIterable &labels) {}
};

class IntUpDownSumObserver : public AsynchronousInstrument
{

public:
  IntUpDownSumObserver() = default;

  IntUpDownSumObserver(nostd::string_view name,
                       nostd::string_view description,
                       nostd::string_view unit,
                       bool enabled,
                       void (*callback)(ObserverResult))
  {}

  virtual void observe(int value, const trace::KeyValueIterable &labels) {}
};

class DoubleUpDownSumObserver : public AsynchronousInstrument
{

public:
  DoubleUpDownSumObserver() = default;

  DoubleUpDownSumObserver(nostd::string_view name,
                          nostd::string_view description,
                          nostd::string_view unit,
                          bool enabled,
                          void (*callback)(ObserverResult))
  {}

  virtual void observe(double value, const trace::KeyValueIterable &labels) {}
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE