// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/metrics/async_instruments.h"
#  include "opentelemetry/metrics/observer_result.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/state/observable_registry.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

class AsyncWritableMetricStorage;

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
                       std::unique_ptr<AsyncWritableMetricStorage> storage);

  void AddCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                   void *state) noexcept override;

  void RemoveCallback(opentelemetry::metrics::ObservableCallbackPtr callback,
                      void *state) noexcept override;

  const InstrumentDescriptor &GetInstrumentDescriptor();

  AsyncWritableMetricStorage *GetMetricStorage();

private:
  InstrumentDescriptor instrument_descriptor_;
  std::unique_ptr<AsyncWritableMetricStorage> storage_;
  std::unique_ptr<ObservableRegistry> observable_registry_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif