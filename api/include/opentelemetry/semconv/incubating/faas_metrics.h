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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricFaasColdstarts(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasColdstarts, descrMetricFaasColdstarts,
                                    unitMetricFaasColdstarts);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasColdstarts(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasColdstarts, descrMetricFaasColdstarts,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricFaasCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasCpuUsage, descrMetricFaasCpuUsage,
                                      unitMetricFaasCpuUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasCpuUsage, descrMetricFaasCpuUsage,
                                               unitMetricFaasCpuUsage);
}

/**
 * Number of invocation errors
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasErrors     = "metric.faas.errors";
static constexpr const char *descrMetricFaasErrors = "Number of invocation errors";
static constexpr const char *unitMetricFaasErrors  = "{error}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricFaasErrors(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasErrors, descrMetricFaasErrors, unitMetricFaasErrors);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasErrors(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasErrors, descrMetricFaasErrors,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricFaasInitDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasInitDuration, descrMetricFaasInitDuration,
                                      unitMetricFaasInitDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasInitDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasInitDuration, descrMetricFaasInitDuration,
                                               unitMetricFaasInitDuration);
}

/**
 * Number of successful invocations
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasInvocations     = "metric.faas.invocations";
static constexpr const char *descrMetricFaasInvocations = "Number of successful invocations";
static constexpr const char *unitMetricFaasInvocations  = "{invocation}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricFaasInvocations(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasInvocations, descrMetricFaasInvocations,
                                    unitMetricFaasInvocations);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasInvocations(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasInvocations, descrMetricFaasInvocations,
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

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricFaasInvokeDuration(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasInvokeDuration, descrMetricFaasInvokeDuration,
                                      unitMetricFaasInvokeDuration);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasInvokeDuration(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(
      kMetricFaasInvokeDuration, descrMetricFaasInvokeDuration, unitMetricFaasInvokeDuration);
}

/**
 * Distribution of max memory usage per invocation
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasMemUsage = "metric.faas.mem_usage";
static constexpr const char *descrMetricFaasMemUsage =
    "Distribution of max memory usage per invocation";
static constexpr const char *unitMetricFaasMemUsage = "By";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricFaasMemUsage(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasMemUsage, descrMetricFaasMemUsage,
                                      unitMetricFaasMemUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasMemUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasMemUsage, descrMetricFaasMemUsage,
                                               unitMetricFaasMemUsage);
}

/**
 * Distribution of net I/O usage per invocation
 * <p>
 * histogram
 */
static constexpr const char *kMetricFaasNetIo     = "metric.faas.net_io";
static constexpr const char *descrMetricFaasNetIo = "Distribution of net I/O usage per invocation";
static constexpr const char *unitMetricFaasNetIo  = "By";

static nostd::unique_ptr<metrics::Histogram<uint64_t>> CreateSyncMetricFaasNetIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Histogram(kMetricFaasNetIo, descrMetricFaasNetIo, unitMetricFaasNetIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasNetIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableHistogram(kMetricFaasNetIo, descrMetricFaasNetIo,
                                               unitMetricFaasNetIo);
}

/**
 * Number of invocation timeouts
 * <p>
 * counter
 */
static constexpr const char *kMetricFaasTimeouts     = "metric.faas.timeouts";
static constexpr const char *descrMetricFaasTimeouts = "Number of invocation timeouts";
static constexpr const char *unitMetricFaasTimeouts  = "{timeout}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricFaasTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricFaasTimeouts, descrMetricFaasTimeouts,
                                    unitMetricFaasTimeouts);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricFaasTimeouts(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricFaasTimeouts, descrMetricFaasTimeouts,
                                             unitMetricFaasTimeouts);
}

}  // namespace faas
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
