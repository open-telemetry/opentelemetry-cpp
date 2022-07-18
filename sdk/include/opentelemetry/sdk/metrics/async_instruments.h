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

class WritableMetricStorage;

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

class ObservableInstrument : public opentelemetry::metrics::ObservableInstrument,
                             public Asynchronous
{
public:
  ObservableInstrument(InstrumentDescriptor instrument_descriptor,
                       std::unique_ptr<WritableMetricStorage> storage)
      : instrument_descriptor_(instrument_descriptor),
        storage_(std::move(storage)),
        observable_registry_{new ObservableRegistry()},
        Asynchronous(instrument_descriptor_.name_,
                     instrument_descriptor.description_,
                     instrument_descriptor_.unit_)
  {}

  void AddCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                   void *state) noexcept override
  {
    observable_registry_->AddCallback(callback, state, this);
  }

  void RemoveCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                      void *state) noexcept override
  {
    observable_registry_->AddCallback(callback, state, this);
  }

  const InstrumentDescriptor &GetInstrumentDescriptor() { return instrument_descriptor_; }

  const WritableMetricStorage *GetMetricStorage() { return storage_.get(); }

private:
protected:
  InstrumentDescriptor instrument_descriptor_;
  std::unique_ptr<WritableMetricStorage> storage_;
  std::unique_ptr<ObservableRegistry> observable_registry_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif