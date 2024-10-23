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
namespace faas
{

/**
 * Number of invocation cold starts
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasColdstarts     = "metric.faas.coldstarts";
static constexpr const char *descrMetricFaasColdstarts = "Number of invocation cold starts";
static constexpr const char *unitMetricFaasColdstarts  = "{coldstart}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricFaasColdstarts(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasColdstarts, descrMetricFaasColdstarts,
                                    unitMetricFaasColdstarts);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricFaasColdstarts(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricFaasColdstarts, descrMetricFaasColdstarts,
                                    unitMetricFaasColdstarts);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricFaasColdstarts(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasColdstarts, descrMetricFaasColdstarts,
                                             unitMetricFaasColdstarts);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricFaasColdstarts(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricFaasColdstarts, descrMetricFaasColdstarts,
                                              unitMetricFaasColdstarts);
}

/**
 * Distribution of CPU usage per invocation
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasCpuUsage     = "metric.faas.cpu_usage";
static constexpr const char *descrMetricFaasCpuUsage = "Distribution of CPU usage per invocation";
static constexpr const char *unitMetricFaasCpuUsage  = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncInt64MetricFaasCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasCpuUsage, descrMetricFaasCpuUsage,
                                      unitMetricFaasCpuUsage);
}

static inline nostd::unique_ptr<metrics::Histogram<double>> CreateSyncDoubleMetricFaasCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricFaasCpuUsage, descrMetricFaasCpuUsage,
                                      unitMetricFaasCpuUsage);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricFaasCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasCpuUsage, descrMetricFaasCpuUsage,
                                               unitMetricFaasCpuUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricFaasCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricFaasCpuUsage, descrMetricFaasCpuUsage,
                                                unitMetricFaasCpuUsage);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Number of invocation errors
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasErrors     = "metric.faas.errors";
static constexpr const char *descrMetricFaasErrors = "Number of invocation errors";
static constexpr const char *unitMetricFaasErrors  = "{error}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricFaasErrors(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasErrors, descrMetricFaasErrors, unitMetricFaasErrors);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricFaasErrors(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricFaasErrors, descrMetricFaasErrors, unitMetricFaasErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricFaasErrors(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasErrors, descrMetricFaasErrors,
                                             unitMetricFaasErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricFaasErrors(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricFaasErrors, descrMetricFaasErrors,
                                              unitMetricFaasErrors);
}

/**
 * Measures the duration of the function's initialization, such as a cold start
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasInitDuration = "metric.faas.init_duration";
static constexpr const char *descrMetricFaasInitDuration =
    "Measures the duration of the function's initialization, such as a cold start";
static constexpr const char *unitMetricFaasInitDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncInt64MetricFaasInitDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasInitDuration, descrMetricFaasInitDuration,
                                      unitMetricFaasInitDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>> CreateSyncDoubleMetricFaasInitDuration(
    metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricFaasInitDuration, descrMetricFaasInitDuration,
                                      unitMetricFaasInitDuration);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricFaasInitDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasInitDuration, descrMetricFaasInitDuration,
                                               unitMetricFaasInitDuration);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricFaasInitDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(
      kMetricFaasInitDuration, descrMetricFaasInitDuration, unitMetricFaasInitDuration);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Number of successful invocations
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasInvocations     = "metric.faas.invocations";
static constexpr const char *descrMetricFaasInvocations = "Number of successful invocations";
static constexpr const char *unitMetricFaasInvocations  = "{invocation}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricFaasInvocations(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasInvocations, descrMetricFaasInvocations,
                                    unitMetricFaasInvocations);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricFaasInvocations(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricFaasInvocations, descrMetricFaasInvocations,
                                    unitMetricFaasInvocations);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricFaasInvocations(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasInvocations, descrMetricFaasInvocations,
                                             unitMetricFaasInvocations);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricFaasInvocations(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricFaasInvocations, descrMetricFaasInvocations,
                                              unitMetricFaasInvocations);
}

/**
 * Measures the duration of the function's logic execution
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasInvokeDuration = "metric.faas.invoke_duration";
static constexpr const char *descrMetricFaasInvokeDuration =
    "Measures the duration of the function's logic execution";
static constexpr const char *unitMetricFaasInvokeDuration = "s";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>>
CreateSyncInt64MetricFaasInvokeDuration(metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasInvokeDuration, descrMetricFaasInvokeDuration,
                                      unitMetricFaasInvokeDuration);
}

static inline nostd::unique_ptr<metrics::Histogram<double>>
CreateSyncDoubleMetricFaasInvokeDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricFaasInvokeDuration, descrMetricFaasInvokeDuration,
                                      unitMetricFaasInvokeDuration);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricFaasInvokeDuration(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricFaasInvokeDuration, descrMetricFaasInvokeDuration, unitMetricFaasInvokeDuration);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricFaasInvokeDuration(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(
      kMetricFaasInvokeDuration, descrMetricFaasInvokeDuration, unitMetricFaasInvokeDuration);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Distribution of max memory usage per invocation
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasMemUsage = "metric.faas.mem_usage";
static constexpr const char *descrMetricFaasMemUsage =
    "Distribution of max memory usage per invocation";
static constexpr const char *unitMetricFaasMemUsage = "By";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncInt64MetricFaasMemUsage(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasMemUsage, descrMetricFaasMemUsage,
                                      unitMetricFaasMemUsage);
}

static inline nostd::unique_ptr<metrics::Histogram<double>> CreateSyncDoubleMetricFaasMemUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricFaasMemUsage, descrMetricFaasMemUsage,
                                      unitMetricFaasMemUsage);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricFaasMemUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasMemUsage, descrMetricFaasMemUsage,
                                               unitMetricFaasMemUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricFaasMemUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricFaasMemUsage, descrMetricFaasMemUsage,
                                                unitMetricFaasMemUsage);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Distribution of net I/O usage per invocation
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasNetIo     = "metric.faas.net_io";
static constexpr const char *descrMetricFaasNetIo = "Distribution of net I/O usage per invocation";
static constexpr const char *unitMetricFaasNetIo  = "By";

static inline nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncInt64MetricFaasNetIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasNetIo, descrMetricFaasNetIo, unitMetricFaasNetIo);
}

static inline nostd::unique_ptr<metrics::Histogram<double>> CreateSyncDoubleMetricFaasNetIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleHistogram(kMetricFaasNetIo, descrMetricFaasNetIo, unitMetricFaasNetIo);
}

#ifdef OPENTELEMETRY_LATER
// Unsupported: Async histogram
static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricFaasNetIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasNetIo, descrMetricFaasNetIo,
                                               unitMetricFaasNetIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricFaasNetIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableHistogram(kMetricFaasNetIo, descrMetricFaasNetIo,
                                                unitMetricFaasNetIo);
}
#endif /* OPENTELEMETRY_LATER */

/**
 * Number of invocation timeouts
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasTimeouts     = "metric.faas.timeouts";
static constexpr const char *descrMetricFaasTimeouts = "Number of invocation timeouts";
static constexpr const char *unitMetricFaasTimeouts  = "{timeout}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricFaasTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasTimeouts, descrMetricFaasTimeouts,
                                    unitMetricFaasTimeouts);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricFaasTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricFaasTimeouts, descrMetricFaasTimeouts,
                                    unitMetricFaasTimeouts);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricFaasTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasTimeouts, descrMetricFaasTimeouts,
                                             unitMetricFaasTimeouts);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricFaasTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricFaasTimeouts, descrMetricFaasTimeouts,
                                              unitMetricFaasTimeouts);
}

}  // namespace faas
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
