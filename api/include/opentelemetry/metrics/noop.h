// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/metrics/async_instruments.h"
#  include "opentelemetry/metrics/meter.h"
#  include "opentelemetry/metrics/meter_provider.h"
#  include "opentelemetry/metrics/observer_result.h"
#  include "opentelemetry/metrics/sync_instruments.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

template <class T>
class NoopCounter : public Counter<T>
{
public:
  NoopCounter(nostd::string_view name,
              nostd::string_view description,
              nostd::string_view unit) noexcept
  {}
  void Add(T value) noexcept override {}
  void Add(T value, const common::KeyValueIterable &attributes) noexcept override {}
};

template <class T>
class NoopHistogram : public Histogram<T>
{
public:
  NoopHistogram(nostd::string_view name,
                nostd::string_view description,
                nostd::string_view unit) noexcept
  {}
  void Record(T value) noexcept override {}
  void Record(T value, const common::KeyValueIterable &attributes) noexcept override {}
};

template <class T>
class NoopUpDownCounter : public UpDownCounter<T>
{
public:
  NoopUpDownCounter(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit) noexcept
  {}
  void Add(T value) noexcept override {}
  void Add(T value, const common::KeyValueIterable &attributes) noexcept override {}
};

template <class T>
class NoopObservableCounter : public ObservableCounter<T>
{
public:
  NoopObservableCounter(nostd::string_view name,
                        void (*callback)(ObserverResult<T> &),
                        nostd::string_view description,
                        nostd::string_view unit) noexcept
  {}

  NoopObservableCounter(nostd::string_view name,
                        void (*callback)(ObserverResult<T> &, const common::KeyValueIterable &),
                        nostd::string_view description,
                        nostd::string_view unit) noexcept
  {}
};

template <class T>
class NoopObservableGauge : public ObservableGauge<T>
{
public:
  NoopObservableGauge(nostd::string_view name,
                      void (*callback)(ObserverResult<T> &),
                      nostd::string_view description,
                      nostd::string_view unit) noexcept
  {}

  NoopObservableGauge(nostd::string_view name,
                      void (*callback)(ObserverResult<T> &, const common::KeyValueIterable &),
                      nostd::string_view description,
                      nostd::string_view unit) noexcept
  {}
};

template <class T>
class NoopObservableUpDownCounter : public ObservableUpDownCounter<T>
{
public:
  NoopObservableUpDownCounter(nostd::string_view name,
                              void (*callback)(ObserverResult<T> &),
                              nostd::string_view description,
                              nostd::string_view unit) noexcept
  {}

  NoopObservableUpDownCounter(nostd::string_view name,
                              void (*callback)(ObserverResult<T> &,
                                               const common::KeyValueIterable &),
                              nostd::string_view description,
                              nostd::string_view unit) noexcept
  {}
};

/**
 * No-op implementation of Meter.
 */
class NoopMeter final : public Meter
{
public:
  nostd::shared_ptr<Counter<long>> CreateLongCounter(nostd::string_view name,
                                                     nostd::string_view description = "",
                                                     nostd::string_view unit = "") noexcept override
  {
    return nostd::shared_ptr<Counter<long>>{new NoopCounter<long>(name, description, unit)};
  }

  nostd::shared_ptr<Counter<double>> CreateDoubleCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<Counter<double>>{new NoopCounter<double>(name, description, unit)};
  }

  nostd::shared_ptr<ObservableCounter<long>> CreateLongObservableCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<ObservableCounter<long>>{
        new NoopObservableCounter<long>(name, callback, description, unit)};
  }

  nostd::shared_ptr<ObservableCounter<double>> CreateDoubleObservableCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<ObservableCounter<double>>{
        new NoopObservableCounter<double>(name, callback, description, unit)};
  }

  nostd::shared_ptr<Histogram<long>> CreateLongHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<Histogram<long>>{new NoopHistogram<long>(name, description, unit)};
  }

  nostd::shared_ptr<Histogram<double>> CreateDoubleHistogram(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<Histogram<double>>{new NoopHistogram<double>(name, description, unit)};
  }

  nostd::shared_ptr<ObservableGauge<long>> CreateLongObservableGauge(
      nostd::string_view name,
      void (*callback)(ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<ObservableGauge<long>>{
        new NoopObservableGauge<long>(name, callback, description, unit)};
  }

  nostd::shared_ptr<ObservableGauge<double>> CreateDoubleObservableGauge(
      nostd::string_view name,
      void (*callback)(ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<ObservableGauge<double>>{
        new NoopObservableGauge<double>(name, callback, description, unit)};
  }

  nostd::shared_ptr<UpDownCounter<long>> CreateLongUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<UpDownCounter<long>>{
        new NoopUpDownCounter<long>(name, description, unit)};
  }

  nostd::shared_ptr<UpDownCounter<double>> CreateDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<UpDownCounter<double>>{
        new NoopUpDownCounter<double>(name, description, unit)};
  }

  nostd::shared_ptr<ObservableUpDownCounter<long>> CreateLongObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<long> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<ObservableUpDownCounter<long>>{
        new NoopObservableUpDownCounter<long>(name, callback, description, unit)};
  }

  nostd::shared_ptr<ObservableUpDownCounter<double>> CreateDoubleObservableUpDownCounter(
      nostd::string_view name,
      void (*callback)(ObserverResult<double> &),
      nostd::string_view description = "",
      nostd::string_view unit        = "") noexcept override
  {
    return nostd::shared_ptr<ObservableUpDownCounter<double>>{
        new NoopObservableUpDownCounter<double>(name, callback, description, unit)};
  }
};

/**
 * No-op implementation of a MeterProvider.
 */
class NoopMeterProvider final : public MeterProvider
{
public:
  NoopMeterProvider() : meter_{nostd::shared_ptr<Meter>(new NoopMeter)} {}

  nostd::shared_ptr<Meter> GetMeter(nostd::string_view library_name,
                                    nostd::string_view library_version,
                                    nostd::string_view schema_url) noexcept override
  {
    return meter_;
  }

private:
  nostd::shared_ptr<Meter> meter_;
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif