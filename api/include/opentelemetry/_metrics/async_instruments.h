// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/_metrics/observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

template <class T>
class ValueObserver : virtual public AsynchronousInstrument<T>
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
  virtual void observe(T value, const common::KeyValueIterable &labels) override = 0;

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

template <class T>
class SumObserver : virtual public AsynchronousInstrument<T>
{

public:
  SumObserver() = default;

  SumObserver(nostd::string_view name,
              nostd::string_view description,
              nostd::string_view unit,
              bool enabled,
              void (*callback)(ObserverResult<T>))
  {}

  virtual void observe(T value, const common::KeyValueIterable &labels) override = 0;

  virtual void run() override = 0;
};

template <class T>
class UpDownSumObserver : virtual public AsynchronousInstrument<T>
{

public:
  UpDownSumObserver() = default;

  UpDownSumObserver(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit,
                    bool enabled,
                    void (*callback)(ObserverResult<T>))
  {}

  virtual void observe(T value, const common::KeyValueIterable &labels) override = 0;

  virtual void run() override = 0;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
