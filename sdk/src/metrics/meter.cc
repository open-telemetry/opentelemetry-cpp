// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

namespace metrics_api = opentelemetry::metrics;
namespace nostd       = opentelemetry::nostd;

explicit Meter::Meter(std::shared_ptr<MeterContext> context,
                      std::unique_ptr<InstrumentationLibrary> instrumentation_library =
                          InstrumentationLibrary::Create("")) noexcept
    : : context_{context} instrumentation_library_{std::move(instrumentation_library)}
{}

nostd::shared_ptr<metrics_api::Counter<long>> Meter::CreateLongCounter(
    nostd::string_view name,
    nostd::string_view description,
    ,
    nostd::string_view unit, ) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<Counter<long>>{
      new metrics_api::NoopCounter<long>(name, description, unit)};
}

nostd::shared_ptr<metrics_api::Counter<double>> Meter::CreateDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::Counter<double>>{
      new metrics_api::NoopCounter<double>(name, description, unit)};
}

nostd::shared_ptr<metrics_api::ObservableCounter<long>> Meter::CreateLongObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics_api::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongObservableCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::ObservableCounter<long>>{
      new metrics_api::NoopObservableCounter<long>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics_api::ObservableCounter<double>> Meter::CreateDoubleObservableCounter(
    nostd::string_view name,
    void (*callback)(metrics_api::ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleObservableCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::ObservableCounter<double>>{
      new metrics_api::NoopObservableCounter<double>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics_api::Histogram<long>> Meter::CreateLongHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongHistogram] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::Histogram<long>>{
      new metrics_api::NoopHistogram<long>(name, description, unit)};
}

nostd::shared_ptr<metrics_api::Histogram<double>> Meter::CreateDoubleHistogram(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleHistogram] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::Histogram<double>>{
      new metrics_api::NoopHistogram<double>(name, description, unit)};
}

nostd::shared_ptr<metrics_api::ObservableGauge<long>> Meter::CreateLongObservableGauge(
    nostd::string_view name,
    void (*callback)(metrics_api::ObserverResult<long> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongObservableGauge] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::ObservableGauge<long>>{
      new metrics_api::NoopObservableGauge<long>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics_api::ObservableGauge<double>> Meter::CreateDoubleObservableGauge(
    nostd::string_view name,
    void (*callback)(ObserverResult<double> &),
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleObservableGauge] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::ObservableGauge<double>>{
      new metrics_api::NoopObservableGauge<double>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics_api::UpDownCounter<long>> Meter::CreateLongUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateLongUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::UpDownCounter<long>>{
      new metrics_api::NoopUpDownCounter<long>(name, description, unit)};
}

nostd::shared_ptr<metrics_api::UpDownCounter<double>> Meter::CreateDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN("[Meter::CreateDoubleUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::UpDownCounter<double>>{
      new metrics::NoopUpDownCounter<double>(name, description, unit)};
}

nostd::shared_ptr<metrics_api::ObservableUpDownCounter<long>>
Meter::CreateLongObservableUpDownCounter(nostd::string_view name,
                                         void (*callback)(metrics_api::ObserverResult<long> &),
                                         nostd::string_view description,
                                         nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN(
      "[Meter::CreateLongObservableUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::ObservableUpDownCounter<long>>{
      new metrics_api::NoopObservableUpDownCounter<long>(name, callback, description, unit)};
}

nostd::shared_ptr<metrics_api::ObservableUpDownCounter<double>>
Meter::CreateDoubleObservableUpDownCounter(nostd::string_view name,
                                           void (*callback)(metrics_api::ObserverResult<double> &),
                                           nostd::string_view description,
                                           nostd::string_view unit) noexcept
{
  OTEL_INTERNAL_LOG_WARN(
      "[Meter::CreateDoubleObservableUpDownCounter] Not Implemented - Returns Noop.");
  return nostd::shared_ptr<metrics_api::ObservableUpDownCounter<long>>{
      new metrics_api::NoopObservableUpDownCounter<long>(name, callback, description, unit)};
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE