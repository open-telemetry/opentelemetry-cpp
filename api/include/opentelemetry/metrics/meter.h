#pragma once

#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
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
//TODO: FIGURE OUT HOW TO LET THE USER CREATE METRIC INSTRUMENTS OF VARIOUS TYPES
template<typename T>
class Meter
{
public:
  virtual ~Meter() = default;

  /**
   * Creates a Counter with the passed characteristics and returns a shared_ptr to that Counter.
   *
   * @param name the name of the new Counter.
   * @param description a brief description of what the Counter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created Counter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument<T>> NewCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  /**
   * Creates an UpDownCounter with the passed characteristics and returns a shared_ptr to that
   * UpDownCounter.
   *
   * @param name the name of the new UpDownCounter.
   * @param description a brief description of what the UpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created UpDownCounter.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument<T>> NewUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  /**
   * Creates a ValueRecorder with the passed characteristics and returns a shared_ptr to that
   * ValueRecorder.
   *
   * @param name the name of the new ValueRecorder.
   * @param description a brief description of what the ValueRecorder is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleValueRecorder.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<SynchronousInstrument<T>> NewValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  /**
   * Creates a SumObserver with the passed characteristics and returns a shared_ptr to that
   * SumObserver.
   *
   * @param name the name of the new SumObserver.
   * @param description a brief description of what the SumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created SumObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument<T>> NewSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<T>)) = 0;

  /**
   * Creates an UpDownSumObserver with the passed characteristics and returns a shared_ptr to
   * that UpDowNSumObserver.
   *
   * @param name the name of the new UpDownSumObserver.
   * @param description a brief description of what the UpDownSumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created UpDownSumObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument<T>> NewUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<T>)) = 0;

  /**
   * Creates a ValueObserver with the passed characteristics and returns a shared_ptr to that
   * ValueObserver.
   *
   * @param name the name of the new ValueObserver.
   * @param description a brief description of what the ValueObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created ValueObserver.
   * @throws NullPointerException if {@code name} is null
   * @throws IllegalArgumentException if a different metric by the same name exists in this meter.
   * @throws IllegalArgumentException if the {@code name} does not match spec requirements.
   */
  virtual opentelemetry::nostd::shared_ptr<AsynchronousInstrument<T>> NewValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<T>)) = 0;

  /**
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * @param labels the set of labels to associate with this recorder.
   * @param values a span of pairs where the first element of the pair is a metric instrument
   * to record to, and the second element is the value to update that instrument with.
   */
  virtual void RecordBatch(
      const trace::KeyValueIterable &labels,
      const nostd::span<std::pair<nostd::shared_ptr<SynchronousInstrument<T>>,
          T>> values) noexcept
  {
    // No-op
  }

  /**
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * This overloads the BatchRecord function to allow {@code labels} to be of type initializer list.
   *
   * @param labels an initializer list holding labels to associate with this recording.
   * @param values a span of pairs where the first element of the pair is a metric instrument
   * to record to, and the second element is the value to update that instrument with.
   */
  void RecordBatch(
      std::initializer_list<std::pair<nostd::string_view, nostd::string_view>> labels,
      const nostd::span<std::pair<nostd::shared_ptr<SynchronousInstrument<T>>,
          T>> values) noexcept
  {
    // No-op
  }

  /**
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * This overloads the BatchRecord function to allow {@code labels} and {@code values}
   * to be of type initializer list.
   *
   * @param labels an initializer list of pairs holding labels to associate with this recording.
   * @param values an initializer list of pairs holding ptrs to instruments and the value to record
   * to that respective instrument.
   */
  void RecordBatch(
      std::initializer_list<std::pair<nostd::string_view, nostd::string_view>> labels,
      std::initializer_list<std::pair<nostd::shared_ptr<SynchronousInstrument<T>>, T>>) noexcept
  {
    // No-op
  }
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
