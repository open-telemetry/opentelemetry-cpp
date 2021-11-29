// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/instrument.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

template <class T>
class BoundCounter : virtual public BoundSynchronousInstrument<T>
{

public:
  BoundCounter() = default;

  BoundCounter(nostd::string_view name,
               nostd::string_view description,
               nostd::string_view unit,
               bool enabled);

  /*
   * Add adds the value to the counter's sum. The labels are already linked   * to the instrument
   * and are not specified.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(T value) = 0;
};

template <class T>
class Counter : virtual public SynchronousInstrument<T>
{

public:
  Counter() = default;

  Counter(nostd::string_view name,
          nostd::string_view description,
          nostd::string_view unit,
          bool enabled)
  {}

  /*
   * Bind creates a bound instrument for this counter. The labels are
   * associated with values recorded via subsequent calls to Record.
   *
   * @param labels the set of labels, as key-value pairs.
   * @return a BoundIntCounter tied to the specified labels
   */
  virtual nostd::shared_ptr<BoundCounter<T>> bindCounter(const common::KeyValueIterable &labels)
  {
    return nostd::shared_ptr<BoundCounter<T>>();
  }

  /*
   * Add adds the value to the counter's sum. The labels should contain
   * the keys and values to be associated with this value.  Counters only     * accept positive
   * valued updates.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(T value, const common::KeyValueIterable &labels) = 0;

  virtual void update(T value, const common::KeyValueIterable &labels) override = 0;
};

template <class T>
class BoundUpDownCounter : virtual public BoundSynchronousInstrument<T>
{

public:
  BoundUpDownCounter() = default;

  BoundUpDownCounter(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled);

  /*
   * Add adds the value to the counter's sum. The labels are already linked to * the instrument and
   * do not need to specified again.  UpDownCounters can accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(T value) = 0;
};

template <class T>
class UpDownCounter : virtual public SynchronousInstrument<T>
{

public:
  UpDownCounter() = default;

  UpDownCounter(nostd::string_view name,
                nostd::string_view description,
                nostd::string_view unit,
                bool enabled);

  virtual nostd::shared_ptr<BoundUpDownCounter<T>> bindUpDownCounter(
      const common::KeyValueIterable &labels)
  {
    return nostd::shared_ptr<BoundUpDownCounter<T>>();
  }

  /*
   * Add adds the value to the counter's sum. The labels should contain
   * the keys and values to be associated with this value.  UpDownCounters can
   * accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(T value, const common::KeyValueIterable &labels) = 0;

  virtual void update(T value, const common::KeyValueIterable &labels) override = 0;
};

template <class T>
class BoundValueRecorder : virtual public BoundSynchronousInstrument<T>
{

public:
  BoundValueRecorder() = default;

  BoundValueRecorder(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled);

  /*
   * Records the value by summing it with previous measurements and checking  * previously stored
   * minimum and maximum values. The labels associated with * new values are already linked to the
   * instrument as it is bound.          * ValueRecorders can accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   */
  virtual void record(T value) = 0;
};

template <class T>
class ValueRecorder : virtual public SynchronousInstrument<T>
{

public:
  ValueRecorder() = default;

  ValueRecorder(nostd::string_view name,
                nostd::string_view description,
                nostd::string_view unit,
                bool enabled);

  virtual nostd::shared_ptr<BoundValueRecorder<T>> bindValueRecorder(
      const common::KeyValueIterable &labels)
  {
    return nostd::shared_ptr<BoundValueRecorder<T>>();
  }

  /*
   * Records the value by summing it with previous measurements and checking  * previously stored
   * minimum and maximum values. The labels should contain the keys and values to be associated with
   * this value.  ValueRecorders can accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void record(T value, const common::KeyValueIterable &labels) = 0;

  virtual void update(T value, const common::KeyValueIterable &labels) override = 0;
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
