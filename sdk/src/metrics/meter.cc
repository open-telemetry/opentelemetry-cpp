// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/meter.h"
#  include "opentelemetry/metrics/noop.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/common/global_log_handler.h"
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
  InstrumentDescriptor instrument_descriptor = {name, description, unit, InstrumentType::kCounter,
                                                InstrumentValueType::kLong};
  auto storage                               = registerMetricStorage(instrument_descriptor);
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::Counter<long>>{
      new metrics::NoopCounter<long>(name, description, unit)};
}

nostd::shared_ptr<metrics::Counter<double>> Meter::CreateDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::Counter<double>>{
      new metrics::NoopCounter<double>(name, description, unit)};
}

nostd::shared_ptr<metrics::ObservableCounter<long>> Meter::CreateLongObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongObservableCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::ObservableCounter<long>>{
      new metrics::NoopObservableCounter<long>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableCounter<double>> Meter::CreateDoubleObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleObservableCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::ObservableCounter<double>>{
      new metrics::NoopObservableCounter<double>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics::Histogram<long>> Meter::CreateLongHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongHistogram] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::Histogram<long>>{
      new metrics::NoopHistogram<long>(name, description, unit)};
}

nostd::shared_ptr<metrics::Histogram<double>> Meter::CreateDoubleHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleHistogram] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::Histogram<double>>{
      new metrics::NoopHistogram<double>(name, description, unit)};
}

nostd::shared_ptr<metrics::ObservableGauge<long>> Meter::CreateLongObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongObservableGauge] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::ObservableGauge<long>>{
      new metrics::NoopObservableGauge<long>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableGauge<double>> Meter::CreateDoubleObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleObservableGauge] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::ObservableGauge<double>>{
      new metrics::NoopObservableGauge<double>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics::UpDownCounter<long>> Meter::CreateLongUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::UpDownCounter<long>>{
      new metrics::NoopUpDownCounter<long>(name, description, unit)};
}

nostd::shared_ptr<metrics::UpDownCounter<double>> Meter::CreateDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::UpDownCounter<double>>{
      new metrics::NoopUpDownCounter<double>(name, description, unit)};
}

nostd::shared_ptr<metrics::ObservableUpDownCounter<long>> Meter::CreateLongObservableUpDownCounter(
    nostd::string_view name,
    void (*callback)(metrics::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN(
      "[Meter::CreateLongObservableUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::ObservableUpDownCounter<long>>{
      new metrics::NoopObservableUpDownCounter<long>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics::ObservableUpDownCounter<double>>
Meter::CreateDoubleObservableUpDownCounter(nostd::string_view name,
                                           void (*callback)(metrics::ObserverResult<double> &),
                                           nostd::string_view description,
                                           nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN(
      "[Meter::CreateDoubleObservableUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics::ObservableUpDownCounter<double>>{
      new metrics::NoopObservableUpDownCounter<double>(name, callback, description, unit)};
}

const sdk::instrumentationlibrary::InstrumentationLibrary &Meter::GetInstrumentationLibrary()
    const noexcept
{
  return *instrumentation_library_;
}

WritableMatricsStorage &Meter::RegisterMetricsStorage(InstrumentDescriptor &instrument_descriptor)
{
  std::vector < std::vector<View *> views;
  context_.GetViewRegistry().FindViews(instrument_descriptor, *instrumentation_library_,
                                       [](const View &view) {
                                         views.push_back(view);
                                       });
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
