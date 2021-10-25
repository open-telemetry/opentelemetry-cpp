// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/metrics_new/async_instruments.h"
#include "opentelemetry/metrics_new/instrument.h"
#include "opentelemetry/metrics_new/sync_instruments.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics_new
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
   */
  virtual nostd::shared_ptr<Counter<short>> CreateShortCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<Counter<int>> CreateIntCounter(nostd::string_view name,
                                                           nostd::string_view description = "",
                                                           nostd::string_view unit        = "",
                                                           const bool enabled = true) noexcept = 0;

  virtual nostd::shared_ptr<Counter<float>> CreateFloatCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<Counter<double>> CreateDoubleCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  /**
   * Creates a Asynchronouse (Observable) counter with the passed characteristics and returns a
   * shared_ptr to that Observable Counter
   *
   * @param name the name of the new Observable Counter.
   * @param description a brief description of what the Observable Counter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created Observable Counter.
   */
  virtual nostd::shared_ptr<ObservableCounter<short>> CreateShortObservableCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<short>),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<ObservableCounter<int>> CreateIntObservableCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<int>),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true, ) noexcept = 0;

  virtual nostd::shared_ptr<ObservableCounter<float>> CreateFloatObservableCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true,
      void (*callback)(ObserverResult<float>)) noexcept = 0;

  virtual nostd::shared_ptr<ObservableCounter<double>> CreateDoubleObservableCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true,
      void (*callback)(ObserverResult<double>)) noexcept = 0;

  /**
   * Creates a Histogram with the passed characteristics and returns a shared_ptr to that Histogram.
   *
   * @param name the name of the new Histogram.
   * @param description a brief description of what the Histogram is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created Histogram.
   */
  virtual nostd::shared_ptr<Histogram<short>> CreateShortHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<Histogram<int>> CreateIntHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<Histogram<float>> CreateFloatHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<Histogram<double>> CreateDoubleHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  /**
   * Creates a Asynchronouse (Observable) Gauge with the passed characteristics and returns a
   * shared_ptr to that Observable Counter
   *
   * @param name the name of the new Observable Gauge.
   * @param description a brief description of what the Observable Gauge is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created Observable Gauge.
   */
  virtual nostd::shared_ptr<ObservableGauge<short>> CreateShortObservableGauge(
      nostd::string_view name,
      void (*callback)(ObserverResult<short>),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<ObservableGauge<int>> CreateIntObservableGauge(
      nostd::string_view name,
      void (*callback)(ObserverResult<int>),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true, ) noexcept = 0;

  virtual nostd::shared_ptr<ObservableGauge<float>> CreateFloatObservableGauge(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) noexcept = 0;

  virtual nostd::shared_ptr<ObservableGauge<double>> CreateDoubleObservableGauge(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) noexcept = 0;

  /**
   * Creates an UpDownCounter with the passed characteristics and returns a shared_ptr to that
   * UpDownCounter.
   *
   * @param name the name of the new UpDownCounter.
   * @param description a brief description of what the UpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created UpDownCounter.
   */
  virtual nostd::shared_ptr<UpDownCounter<short>> CreateShortUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<UpDownCounter<int>> CreateIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) = 0;

  virtual nostd::shared_ptr<UpDownCounter<float>> CreateFloatUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<UpDownCounter<double>> CreateDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  /**
   * Creates a Asynchronouse (Observable) UpDownCounter with the passed characteristics and returns
   * a shared_ptr to that Observable UpDownCounter
   *
   * @param name the name of the new Observable UpDownCounter.
   * @param description a brief description of what the Observable UpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created Observable UpDownCounter.
   */
  virtual nostd::shared_ptr<ObservableUpDownCounter<short>> CreateShortObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<short>),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true) noexcept = 0;

  virtual nostd::shared_ptr<ObservableUpDownCounter<int>> CreateIntObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<int>),
      nostd::string_view description = "",
      nostd::string_view unit        = "",
      const bool enabled             = true, ) noexcept = 0;

  virtual nostd::shared_ptr<ObservableUpDownCounter<float>> CreateFloatObservableUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) noexcept = 0;

  virtual nostd::shared_ptr<ObservableUpDownCounter<double>> CreateDoubleObservableUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) noexcept = 0;
};
}  // namespace metrics_new
OPENTELEMETRY_END_NAMESPACE
