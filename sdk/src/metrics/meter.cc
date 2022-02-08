// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/metrics/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/metrics/async_instruments.h"
#  include "opentelemetry/sdk/metrics/sync_instruments.h"

#  include "opentelemetry/version.h"

#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace metrics = opentelemetry::metrics;
namespace nostd   = opentelemetry::nostd;

Meter::Meter(std::shared_ptr<MeterContext> context,
             std::unique_ptr<sdk::instrumentationlibrary::InstrumentationLibrary>
                 instrumentation_library) noexcept
    : context_{context}, instrumentation_library_{std::move(instrumentation_library)}
{}

nostd::shared_ptr<metrics::Counter<long>> Meter::CreateLongCounter(nostd::string_view name,
                                                                   nostd::string_view description,
                                                                   nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::Counter<long>>(new LongCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), description, unit));
}

nostd::shared_ptr<metrics::Counter<double>> Meter::CreateDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::Counter<double>>{new DoubleCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), description, unit)};
}

nostd::shared_ptr<metrics::ObservableCounter<long>> Meter::CreateLongObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableCounter<long>>{new LongObservableCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableCounter<double>> Meter::CreateDoubleObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableCounter<double>>{new DoubleObservableCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), callback, description, unit)};
}

nostd::shared_ptr<metrics::Histogram<long>> Meter::CreateLongHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::Histogram<long>>{new LongHistogram(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), description, unit)};
}

nostd::shared_ptr<metrics::Histogram<double>> Meter::CreateDoubleHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::Histogram<double>>{new DoubleHistogram(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), description, unit)};
}

nostd::shared_ptr<metrics::ObservableGauge<long>> Meter::CreateLongObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableGauge<long>>{new LongObservableGauge(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableGauge<double>> Meter::CreateDoubleObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableGauge<double>>{new DoubleObservableGauge(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), callback, description, unit)};
}

nostd::shared_ptr<metrics::UpDownCounter<long>> Meter::CreateLongUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::UpDownCounter<long>>{new LongUpDownCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), description, unit)};
}

nostd::shared_ptr<metrics::UpDownCounter<double>> Meter::CreateDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::UpDownCounter<double>>{new DoubleUpDownCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), description, unit)};
}

nostd::shared_ptr<metrics::ObservableUpDownCounter<long>> Meter::CreateLongObservableUpDownCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableUpDownCounter<long>>{new LongObservableUpDownCounter(
      name, GetInstrumentationLibrary(), GetMeasurementProcessor(), callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableUpDownCounter<double>>
Meter::CreateDoubleObservableUpDownCounter(nostd::string_view name,
                                           void (*callback)(metrics::ObserverResult<double> &),
                                           nostd::string_view description,
                                           nostd::string_view unit) noexcept
{
  return nostd::shared_ptr<metrics::ObservableUpDownCounter<double>>{
      new DoubleObservableUpDownCounter(name, GetInstrumentationLibrary(),
                                        GetMeasurementProcessor(), callback, description, unit)};
}

const sdk::instrumentationlibrary::InstrumentationLibrary *Meter::GetInstrumentationLibrary()
    const noexcept
{
  return instrumentation_library_.get();
}

MeasurementProcessor *Meter::GetMeasurementProcessor() const noexcept
{
  return context_->GetMeasurementProcessor();
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
