#pragma once

#include "instrument.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

class BoundIntCounter : public BoundSynchronousInstrument
{

public:
  BoundIntCounter() = default;

  BoundIntCounter(nostd::string_view name,
                  nostd::string_view description,
                  nostd::string_view unit,
                  bool enabled)
  {}

  /*
   * Add adds the value to the counter's sum. The labels are already linked   * to the instrument
   * and are not specified.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(int value) {}
};

class IntCounter : public SynchronousInstrument
{

public:
  IntCounter() = default;

  IntCounter(nostd::string_view name,
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
  nostd::shared_ptr<BoundIntCounter> bind(const nostd::string_view &labels)
  {
    return nostd::shared_ptr<BoundIntCounter>(new BoundIntCounter());
  }

  /*
   * Add adds the value to the counter's sum. The labels should contain
   * the keys and values to be associated with this value.  Counters only     * accept positive
   * valued updates.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(int value, const nostd::string_view &labels) {}
};

class BoundDoubleCounter : public BoundSynchronousInstrument
{

public:
  BoundDoubleCounter() = default;

  BoundDoubleCounter(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled)
  {}

  virtual void add(double value) {}
};

class DoubleCounter : public SynchronousInstrument
{

public:
  DoubleCounter() = default;

  DoubleCounter(nostd::string_view name,
                nostd::string_view description,
                nostd::string_view unit,
                bool enabled)
  {}

  nostd::shared_ptr<BoundDoubleCounter> bind(const nostd::string_view &labels)
  {
    return nostd::shared_ptr<BoundDoubleCounter>(new BoundDoubleCounter());
  }

  virtual void add(double value, const nostd::string_view &labels) {}
};

class BoundIntUpDownCounter : public BoundSynchronousInstrument
{

public:
  BoundIntUpDownCounter() = default;

  BoundIntUpDownCounter(nostd::string_view name,
                        nostd::string_view description,
                        nostd::string_view unit,
                        bool enabled)
  {}

  /*
   * Add adds the value to the counter's sum. The labels are already linked to * the instrument and
   * do not need to specified again.  UpDownCounters can accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(int value) {}
};

class IntUpDownCounter : public SynchronousInstrument
{

public:
  IntUpDownCounter() = default;

  IntUpDownCounter(nostd::string_view name,
                   nostd::string_view description,
                   nostd::string_view unit,
                   bool enabled)
  {}

  nostd::shared_ptr<BoundIntUpDownCounter> bind(const nostd::string_view &labels)
  {
    return nostd::shared_ptr<BoundIntUpDownCounter>(new BoundIntUpDownCounter());
  }

  /*
   * Add adds the value to the counter's sum. The labels should contain
   * the keys and values to be associated with this value.  UpDownCounters can
   * accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void add(int value, const nostd::string_view &labels) {}
};

class BoundDoubleUpDownCounter : public BoundSynchronousInstrument
{

public:
  BoundDoubleUpDownCounter() = default;

  BoundDoubleUpDownCounter(nostd::string_view name,
                           nostd::string_view description,
                           nostd::string_view unit,
                           bool enabled)
  {}

  virtual void add(double value) {}
};

class DoubleUpDownCounter : public SynchronousInstrument
{

public:
  DoubleUpDownCounter() = default;

  DoubleUpDownCounter(nostd::string_view name,
                      nostd::string_view description,
                      nostd::string_view unit,
                      bool enabled)
  {}

  nostd::shared_ptr<BoundDoubleUpDownCounter> bind(const nostd::string_view &labels)
  {
    return nostd::shared_ptr<BoundDoubleUpDownCounter>(new BoundDoubleUpDownCounter());
  }

  virtual void add(double value, const nostd::string_view &labels) {}
};

class BoundIntValueRecorder : public BoundSynchronousInstrument
{

public:
  BoundIntValueRecorder() = default;

  BoundIntValueRecorder(nostd::string_view name,
                        nostd::string_view description,
                        nostd::string_view unit,
                        bool enabled)
  {}

  /*
   * Records the value by summing it with previous measurements and checking  * previously stored
   * minimum and maximum values. The labels associated with * new values are already linked to the
   * instrument as it is bound.          * ValueRecorders can accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   */
  virtual void record(int value) {}
};

class IntValueRecorder : public SynchronousInstrument
{

public:
  IntValueRecorder() = default;

  IntValueRecorder(nostd::string_view name,
                   nostd::string_view description,
                   nostd::string_view unit,
                   bool enabled)
  {}

  nostd::shared_ptr<BoundIntValueRecorder> bind(const nostd::string_view &labels)
  {
    return nostd::shared_ptr<BoundIntValueRecorder>(new BoundIntValueRecorder());
  }

  /*
   * Records the value by summing it with previous measurements and checking  * previously stored
   * minimum and maximum values. The labels should contain the keys and values to be associated with
   * this value.  ValueRecorders can accept positive and negative values.
   *
   * @param value the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void record(int value, const nostd::string_view &labels) {}
};

class BoundDoubleValueRecorder : public BoundSynchronousInstrument
{

public:
  BoundDoubleValueRecorder() = default;

  BoundDoubleValueRecorder(nostd::string_view name,
                           nostd::string_view description,
                           nostd::string_view unit,
                           bool enabled)
  {}

  virtual void record(double value) {}
};

class DoubleValueRecorder : public SynchronousInstrument
{

public:
  DoubleValueRecorder() = default;

  DoubleValueRecorder(nostd::string_view name,
                      nostd::string_view description,
                      nostd::string_view unit,
                      bool enabled)
  {}

  nostd::shared_ptr<BoundDoubleValueRecorder> bind(const nostd::string_view &labels)
  {
    return nostd::shared_ptr<BoundDoubleValueRecorder>(new BoundDoubleValueRecorder());
  }

  virtual void record(double value, const nostd::string_view &labels) {}
};

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE