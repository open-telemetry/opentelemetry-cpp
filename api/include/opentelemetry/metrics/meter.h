#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/version.h"

#include "opentelemetry/metrics/async_instruments.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/metrics/sync_instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{
/**
 * Handles instrument creation and provides a facility for batch recording.
 *
 * This class provides methods to create new metric instruments, record a
 * batch of values to a specified set of instruments, and collect
 * measurements from all instruments.
 *
 */
class Meter
{
public:
  virtual ~Meter() = default;

  //////////////////////////////
  //                          //
  // Change pointer type to   //
  // SynchronousInstrument or //
  // AsynchronousInstrument?  //
  //                          //
  //////////////////////////////

  /**
   * Creates, adds to private metrics container, and returns a DoubleCounter with "name."
   *
   * @param name the name of the new DoubleCounter.
   * @param description a brief description of what the DoubleCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleCounter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewDoubleCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled)
  {
    return opentelemetry::nostd::shared_ptr<SynchronousInstrument>{
        new DoubleCounter(name, description, unit, enabled)};
  }

  /**
   * Creates, adds to private metrics container, and returns an IntCounter with "name."
   *
   * @param name the name of the new IntCounter.
   * @param description a brief description of what the IntCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created IntCounter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewIntCounter(nostd::string_view name,
                                                                     nostd::string_view description,
                                                                     nostd::string_view unit,
                                                                     const bool enabled)
  {
    return opentelemetry::nostd::shared_ptr<SynchronousInstrument>{
        new IntCounter(name, description, unit, enabled)};
  }

  /**
   * Creates, adds to private metrics container, and returns a DoubleUpDownCounter with "name."
   *
   * @param name the name of the new DoubleUpDownCounter.
   * @param description a brief description of what the DoubleUpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleUpDownCounter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled)
  {
    return opentelemetry::nostd::shared_ptr<SynchronousInstrument>{
        new DoubleUpDownCounter(name, description, unit, enabled)};
  }

  /**
   * Creates, adds to private metrics container, and returns an IntUpDownCounter with "name."
   *
   * @param name the name of the new IntUpDownCounter.
   * @param description a brief description of what the IntUpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created IntUpDownCounter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled)
  {
    return opentelemetry::nostd::shared_ptr<SynchronousInstrument>{
        new IntUpDownCounter(name, description, unit, enabled)};
  }

  /**
   * Creates, adds to private metrics container, and returns a DoubleValueRecorder with "name."
   *
   * @param name the name of the new DoubleValueRecorder.
   * @param description a brief description of what the DoubleValueRecorder is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleValueRecorder.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled)
  {
    return opentelemetry::nostd::shared_ptr<SynchronousInstrument>{
        new DoubleValueRecorder(name, description, unit, enabled)};
  }

  /**
   * Creates, adds to private metrics container, and returns an IntValueRecorder with "name."
   *
   * @param name the name of the new IntValueRecorder.
   * @param description a brief description of what the IntValueRecorder is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created IntValueRecorder.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      bool enabled)
  {
    return opentelemetry::nostd::shared_ptr<SynchronousInstrument>{
        new IntValueRecorder(name, description, unit, enabled)};
  }

  /**
   * Creates, adds to private metrics container, and returns a DoubleSumObserver with "name."
   *
   * @param name the name of the new DoubleSumObserver.
   * @param description a brief description of what the DoubleSumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleSumObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult))
  {
    return opentelemetry::nostd::shared_ptr<AsynchronousInstrument>{
        new DoubleSumObserver(name, description, unit, enabled, callback)};
  }

  /**
   * Creates, adds to private metrics container, and returns an IntSumObserver with "name."
   *
   * @param name the name of the new IntSumObserver.
   * @param description a brief description of what the IntSumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleCounter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult))
  {
    return opentelemetry::nostd::shared_ptr<AsynchronousInstrument>{
        new IntSumObserver(name, description, unit, enabled, callback)};
  }

  /**
   * Creates, adds to private metrics container, and returns a DoubleUpDownSumObserver with "name."
   *
   * @param name the name of the new DoubleUpDownSumObserver.
   * @param description a brief description of what the DoubleUpDownSumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleUpDownSumObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument>
  NewDoubleUpDownSumObserver(nostd::string_view name,
                             nostd::string_view description,
                             nostd::string_view unit,
                             const bool enabled,
                             void (*callback)(DoubleObserverResult))
  {
    return opentelemetry::nostd::shared_ptr<AsynchronousInstrument>{
        new DoubleUpDownSumObserver(name, description, unit, enabled, callback)};
  }

  /**
   * Creates, adds to private metrics container, and returns an IntUpDownSumObserver with "name."
   *
   * @param name the name of the new IntUpDownSumObserver.
   * @param description a brief description of what the IntUpDownSumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created IntUpDownSumObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult))
  {
    return opentelemetry::nostd::shared_ptr<AsynchronousInstrument>{
        new IntUpDownSumObserver(name, description, unit, enabled, callback)};
  }

  /**
   * Creates, adds to private metrics container, and returns a DoubleValueObserver with "name."
   *
   * @param name the name of the new DoubleValueObserver.
   * @param description a brief description of what the DoubleValueObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleValueObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult))
  {
    return opentelemetry::nostd::shared_ptr<AsynchronousInstrument>{
        new DoubleValueObserver(name, description, unit, enabled, callback)};
  }

  /**
   * Creates, adds to private metrics container, and returns an IntValueObserver with "name."
   *
   * @param name the name of the new IntValueObserver.
   * @param description a brief description of what the IntValueObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created IntValueObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewIntValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult))
  {
    return opentelemetry::nostd::shared_ptr<AsynchronousInstrument>{
        new IntValueObserver(name, description, unit, enabled, callback)};
  }

  /**
   * Utility method that allows users to atomically record measurements to a set of
   * Metric instruments with a common set of labels.
   *
   * @param labels The set of labels to associate with this recorder.
   * @param values A KeyValueIterable where the key is a string containing the name
   *        of metric instruments such as "IntCounter" or "DoubleUpDownSumObserver"
   *        and the value is the value to be recorded to all metric instruments in
   *        the batch of the associated instrument type.
   */
  virtual void RecordBatch(nostd::string_view labels,
                           const trace::KeyValueIterable &values) noexcept
  {
    // No-op
  }
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
