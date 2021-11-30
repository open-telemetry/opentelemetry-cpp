// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/metrics/async_instruments.h"
#  include "opentelemetry/metrics/sync_instruments.h"
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
   * @return a shared pointer to the created Counter.
   */

  virtual nostd::shared_ptr<Counter<long>> CreateLongCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  virtual nostd::shared_ptr<Counter<double>> CreateDoubleCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  /**
   * Creates a Asynchronouse (Observable) counter with the passed characteristics and returns a
   * shared_ptr to that Observable Counter
   *
   * @param name the name of the new Observable Counter.
   * @param description a brief description of what the Observable Counter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created Observable Counter.
   */
  virtual nostd::shared_ptr<ObservableCounter<long>> CreateLongObservableCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  virtual nostd::shared_ptr<ObservableCounter<double>> CreateDoubleObservableCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  /**
   * Creates a Histogram with the passed characteristics and returns a shared_ptr to that Histogram.
   *
   * @param name the name of the new Histogram.
   * @param description a brief description of what the Histogram is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @return a shared pointer to the created Histogram.
   */
  virtual nostd::shared_ptr<Histogram<long>> CreateLongHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  virtual nostd::shared_ptr<Histogram<double>> CreateDoubleHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  /**
   * Creates a Asynchronouse (Observable) Gauge with the passed characteristics and returns a
   * shared_ptr to that Observable Counter
   *
   * @param name the name of the new Observable Gauge.
   * @param description a brief description of what the Observable Gauge is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created Observable Gauge.
   */
  virtual nostd::shared_ptr<ObservableGauge<long>> CreateLongObservableGauge(
      nostd::string_view name,
      void (*callback)(ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  virtual nostd::shared_ptr<ObservableGauge<double>> CreateDoubleObservableGauge(
      nostd::string_view name,
      void (*callback)(ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  /**
   * Creates an UpDownCounter with the passed characteristics and returns a shared_ptr to that
   * UpDownCounter.
   *
   * @param name the name of the new UpDownCounter.
   * @param description a brief description of what the UpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @return a shared pointer to the created UpDownCounter.
   */
  virtual nostd::shared_ptr<UpDownCounter<long>> CreateLongUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  virtual nostd::shared_ptr<UpDownCounter<double>> CreateDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  /**
   * Creates a Asynchronouse (Observable) UpDownCounter with the passed characteristics and returns
   * a shared_ptr to that Observable UpDownCounter
   *
   * @param name the name of the new Observable UpDownCounter.
   * @param description a brief description of what the Observable UpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created Observable UpDownCounter.
   */
  virtual nostd::shared_ptr<ObservableUpDownCounter<long>> CreateLongObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;

  virtual nostd::shared_ptr<ObservableUpDownCounter<double>> CreateDoubleObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept = 0;
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
