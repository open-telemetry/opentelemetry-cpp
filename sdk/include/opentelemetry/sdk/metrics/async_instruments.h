// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/metrics/async_instruments.h"
#  include "opentelemetry/metrics/sdk/instruments.h"
#  include "opentelemetry/nostd/string_view.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
typedef void (*AsyncCallback)(ObserverResult<long> &);
class Asynchronous
{
public:
  Asynchrounous(nostd::string_view name,
                InstrumentDescriptor &descriptor,
                AsyncCallback callback,
                nostd::string_view description = "",
                nostd::string_view unit        = "")
      : name_(name),
        instrument_descriptor_(descriptor),
        callback_(callback),
        description_(description),
        unit_(unit)
  {}

  nostd::string_view GetName() { return name_; }

  nostd::string_view GetDescription() { return description_; }

  nostd::string_unit GetUnit() { return unit_; }

  AsyncCallback GetCallback() { return callback_; }

private:
  std::string name_;
  std::string description_;
  std::string unit_;
  InstrumentDescriptor instrument_descriptor_;
  AsyncCallback callback_;
};

class LongObservableCounter : public opentelemetry::metrics::ObservableCounter<long>
{
  LongObservableCounter(nostd::string_view name,
                        InstrumentDescriptor &descriptor,
                        AsyncCallback callback,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, descriptor, callback, description, unit);

  {}
};

class DoubleObservableCounter : public opentelemetry::metrics::ObservableCounter<double>
{
  LongObservableCounter(nostd::string_view name,
                        InstrumentDescriptor &descriptor,
                        AsyncCallback callback,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, descriptor, callback, description, unit);

  {}
};

class LongObservableGauge : public opentelemetry::metrics::ObservableGauge<long>
{
  LongObservableCounter(nostd::string_view name,
                        InstrumentDescriptor &descriptor,
                        AsyncCallback callback,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, descriptor, callback, description, unit);

  {}
};

class DoubleObservableGauge : public opentelemetry::metrics::ObservableGauge<double>
{
  LongObservableCounter(nostd::string_view name,
                        InstrumentDescriptor &descriptor,
                        AsyncCallback callback,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, descriptor, callback, description, unit);

  {}
};

class LongObservableUpDownCounter : public opentelemetry::metrics::ObservableUpDownCounter<long>
{
  LongObservableCounter(nostd::string_view name,
                        InstrumentDescriptor &descriptor,
                        AsyncCallback callback,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, descriptor, callback, description, unit);

  {}
};

class DoubleObservableUpDownCounter : public opentelemetry::metrics::ObservableUpDownCounter<double>
{
  LongObservableCounter(nostd::string_view name,
                        InstrumentDescriptor &descriptor,
                        AsyncCallback callback,
                        nostd::string_view description = "",
                        nostd::string_view unit        = "")
      : Asynchronous(name, descriptor, callback, description, unit);

  {}
};

}  // namespace metrics
}  // namespace sdk
}
OPENTELEMETRY_END_NAMESPACE
#endif