/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * DO NOT EDIT, this is an Auto-generated file from:
 * buildscripts/semantic-convention/templates/registry/semantic_metrics-h.j2
 */

#pragma once

#include "opentelemetry/common/macros.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace semconv
{
namespace hw
{

/**
 * Energy consumed by the component
 * <p>
 * counter
 */
static constexpr const char *kMetricHwEnergy     = "metric.hw.energy";
static constexpr const char *descrMetricHwEnergy = "Energy consumed by the component";
static constexpr const char *unitMetricHwEnergy  = "J";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricHwEnergy(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricHwEnergy, descrMetricHwEnergy, unitMetricHwEnergy);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricHwEnergy(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricHwEnergy, descrMetricHwEnergy, unitMetricHwEnergy);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricHwEnergy(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricHwEnergy, descrMetricHwEnergy,
                                             unitMetricHwEnergy);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricHwEnergy(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricHwEnergy, descrMetricHwEnergy,
                                              unitMetricHwEnergy);
}

/**
 * Number of errors encountered by the component
 * <p>
 * counter
 */
static constexpr const char *kMetricHwErrors     = "metric.hw.errors";
static constexpr const char *descrMetricHwErrors = "Number of errors encountered by the component";
static constexpr const char *unitMetricHwErrors  = "{error}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricHwErrors(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricHwErrors, descrMetricHwErrors, unitMetricHwErrors);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricHwErrors(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricHwErrors, descrMetricHwErrors, unitMetricHwErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricHwErrors(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricHwErrors, descrMetricHwErrors,
                                             unitMetricHwErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricHwErrors(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricHwErrors, descrMetricHwErrors,
                                              unitMetricHwErrors);
}

/**
 * Instantaneous power consumed by the component
 * <p>
 * It is recommended to report @code hw.energy @endcode instead of @code hw.power @endcode when
 * possible. <p> gauge
 */
static constexpr const char *kMetricHwPower     = "metric.hw.power";
static constexpr const char *descrMetricHwPower = "Instantaneous power consumed by the component";
static constexpr const char *unitMetricHwPower  = "W";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricHwPower(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricHwPower, descrMetricHwPower, unitMetricHwPower);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricHwPower(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricHwPower, descrMetricHwPower, unitMetricHwPower);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricHwPower(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricHwPower, descrMetricHwPower, unitMetricHwPower);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricHwPower(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricHwPower, descrMetricHwPower, unitMetricHwPower);
}

/**
 * Operational status: @code 1 @endcode (true) or @code 0 @endcode (false) for each of the possible
 * states <p>
 * @code hw.status @endcode is currently specified as an <em>UpDownCounter</em> but would ideally be
 * represented using a <a
 * href="https://github.com/OpenObservability/OpenMetrics/blob/main/specification/OpenMetrics.md#stateset"><em>StateSet</em>
 * as defined in OpenMetrics</a>. This semantic convention will be updated once <em>StateSet</em> is
 * specified in OpenTelemetry. This planned change is not expected to have any consequence on the
 * way users query their timeseries backend to retrieve the values of @code hw.status @endcode over
 * time. <p> updowncounter
 */
static constexpr const char *kMetricHwStatus = "metric.hw.status";
static constexpr const char *descrMetricHwStatus =
    "Operational status: `1` (true) or `0` (false) for each of the possible states";
static constexpr const char *unitMetricHwStatus = "1";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>> CreateSyncInt64MetricHwStatus(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricHwStatus, descrMetricHwStatus, unitMetricHwStatus);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>> CreateSyncDoubleMetricHwStatus(
    metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricHwStatus, descrMetricHwStatus, unitMetricHwStatus);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricHwStatus(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricHwStatus, descrMetricHwStatus,
                                                   unitMetricHwStatus);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricHwStatus(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricHwStatus, descrMetricHwStatus,
                                                    unitMetricHwStatus);
}

}  // namespace hw
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
