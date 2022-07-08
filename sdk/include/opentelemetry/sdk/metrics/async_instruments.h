// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/metrics/async_instruments.h"
#  include "opentelemetry/metrics/observer_result.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template <class T>
class ObservableInstrument
{
public:
  /**
   * Sets up a function that will be called whenever a metric collection is initiated.
   */
  virtual void AddCallback(opentelemetry::metrics::ObservableCallbackPtr, void *state);

  /**
   * Sets up a function that will be called whenever a metric collection is initiated.
   */
  virtual void RemoveCallback(opentelemetry::metrics::ObservableCallbackPtr, void *state);
};

template <class T>
class Asynchronous
{
public:
  Asynchronous(nostd::string_view name,
               nostd::string_view description = "",
               nostd::string_view unit        = "")
      : name_(name), description_(description), unit_(unit)
  {}

protected:
  std::string name_;
  std::string description_;
  std::string unit_;
};

class LongObservableCounter : public opentelemetry::metrics::ObservableCounter<long>,
                              public Asynchronous<long>
{
public:
  LongObservableCounter(nostd::string_view name,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, description, unit)

  {}
};

class DoubleObservableCounter : public opentelemetry::metrics::ObservableCounter<double>,
                                public Asynchronous<double>
{
public:
  DoubleObservableCounter(nostd::string_view name,
                          nostd::string_view description = "",
                          nostd::string_view unit        = "")
      : Asynchronous(name, description, unit)

  {}
};

class LongObservableGauge : public opentelemetry::metrics::ObservableGauge<long>,
                            public Asynchronous<long>
{
public:
  LongObservableGauge(nostd::string_view name,
                      nostd::string_view description = "",
                      nostd::string_view unit        = "")
      : Asynchronous(name, description, unit)

  {}
};

class DoubleObservableGauge : public opentelemetry::metrics::ObservableGauge<double>,
                              public Asynchronous<double>
{
public:
  DoubleObservableGauge(nostd::string_view name,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, description, unit)

  {}
};

class LongObservableUpDownCounter : public opentelemetry::metrics::ObservableUpDownCounter<long>,
                                    public Asynchronous<long>
{
public:
  LongObservableUpDownCounter(nostd::string_view name,
                              nostd::string_view description = "",
                              nostd::string_view unit        = "")
      : Asynchronous(name, description, unit)

  {}
};

class DoubleObservableUpDownCounter
    : public opentelemetry::metrics::ObservableUpDownCounter<double>,
      public Asynchronous<double>
{
public:
  DoubleObservableUpDownCounter(nostd::string_view name,
                                nostd::string_view description = "",
                                nostd::string_view unit        = "")
      : Asynchronous(name, description, unit)
  {}
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif