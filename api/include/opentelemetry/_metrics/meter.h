// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/async_instruments.h"
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/_metrics/sync_instruments.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/nostd/span.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/version.h"

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
  virtual nostd::shared_ptr<Counter<short>> NewShortCounter(nostd::string_view name,
                                                            nostd::string_view description,
                                                            nostd::string_view unit,
                                                            const bool enabled) = 0;

  virtual nostd::shared_ptr<Counter<int>> NewIntCounter(nostd::string_view name,
                                                        nostd::string_view description,
                                                        nostd::string_view unit,
                                                        const bool enabled) = 0;

  virtual nostd::shared_ptr<Counter<float>> NewFloatCounter(nostd::string_view name,
                                                            nostd::string_view description,
                                                            nostd::string_view unit,
                                                            const bool enabled) = 0;

  virtual nostd::shared_ptr<Counter<double>> NewDoubleCounter(nostd::string_view name,
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
  virtual nostd::shared_ptr<UpDownCounter<short>> NewShortUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  virtual nostd::shared_ptr<UpDownCounter<int>> NewIntUpDownCounter(nostd::string_view name,
                                                                    nostd::string_view description,
                                                                    nostd::string_view unit,
                                                                    const bool enabled) = 0;

  virtual nostd::shared_ptr<UpDownCounter<float>> NewFloatUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  virtual nostd::shared_ptr<UpDownCounter<double>> NewDoubleUpDownCounter(
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
  virtual nostd::shared_ptr<ValueRecorder<short>> NewShortValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  virtual nostd::shared_ptr<ValueRecorder<int>> NewIntValueRecorder(nostd::string_view name,
                                                                    nostd::string_view description,
                                                                    nostd::string_view unit,
                                                                    const bool enabled) = 0;

  virtual nostd::shared_ptr<ValueRecorder<float>> NewFloatValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) = 0;

  virtual nostd::shared_ptr<ValueRecorder<double>> NewDoubleValueRecorder(
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
  virtual nostd::shared_ptr<SumObserver<short>> NewShortSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) = 0;

  virtual nostd::shared_ptr<SumObserver<int>> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) = 0;

  virtual nostd::shared_ptr<SumObserver<float>> NewFloatSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) = 0;

  virtual nostd::shared_ptr<SumObserver<double>> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) = 0;

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
  virtual nostd::shared_ptr<UpDownSumObserver<short>> NewShortUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) = 0;

  virtual nostd::shared_ptr<UpDownSumObserver<int>> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) = 0;

  virtual nostd::shared_ptr<UpDownSumObserver<float>> NewFloatUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) = 0;

  virtual nostd::shared_ptr<UpDownSumObserver<double>> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) = 0;

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
  virtual nostd::shared_ptr<ValueObserver<short>> NewShortValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) = 0;

  virtual nostd::shared_ptr<ValueObserver<int>> NewIntValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) = 0;

  virtual nostd::shared_ptr<ValueObserver<float>> NewFloatValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) = 0;

  virtual nostd::shared_ptr<ValueObserver<double>> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) = 0;

  /**
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * @param labels the set of labels to associate with this recorder.
   * @param instruments a span of pointers to the instruments to record to.
   * @param values a span of values to record to the instruments in the corresponding
   * position in the instruments span.
   */
  virtual void RecordShortBatch(const common::KeyValueIterable &labels,
                                nostd::span<SynchronousInstrument<short> *> instruments,
                                nostd::span<const short> values) noexcept = 0;

  virtual void RecordIntBatch(const common::KeyValueIterable &labels,
                              nostd::span<SynchronousInstrument<int> *> instruments,
                              nostd::span<const int> values) noexcept = 0;

  virtual void RecordFloatBatch(const common::KeyValueIterable &labels,
                                nostd::span<SynchronousInstrument<float> *> instruments,
                                nostd::span<const float> values) noexcept = 0;

  virtual void RecordDoubleBatch(const common::KeyValueIterable &labels,
                                 nostd::span<SynchronousInstrument<double> *> instruments,
                                 nostd::span<const double> values) noexcept = 0;
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
