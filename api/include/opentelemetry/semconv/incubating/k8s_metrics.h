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
namespace k8s
{

/**
 * Total CPU time consumed
 * <p>
 * Total CPU time consumed by the specific Node on all available CPU cores
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sNodeCpuTime     = "k8s.node.cpu.time";
static constexpr const char *descrMetricK8sNodeCpuTime = "Total CPU time consumed";
static constexpr const char *unitMetricK8sNodeCpuTime  = "s";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricK8sNodeCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricK8sNodeCpuTime, descrMetricK8sNodeCpuTime,
                                    unitMetricK8sNodeCpuTime);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricK8sNodeCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricK8sNodeCpuTime, descrMetricK8sNodeCpuTime,
                                    unitMetricK8sNodeCpuTime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricK8sNodeCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricK8sNodeCpuTime, descrMetricK8sNodeCpuTime,
                                             unitMetricK8sNodeCpuTime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sNodeCpuTime(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricK8sNodeCpuTime, descrMetricK8sNodeCpuTime,
                                              unitMetricK8sNodeCpuTime);
}

/**
 * Node's CPU usage, measured in cpus. Range from 0 to the number of allocatable CPUs
 * <p>
 * CPU usage of the specific Node on all available CPU cores, averaged over the sample window
 * <p>
 * gauge
 */
static constexpr const char *kMetricK8sNodeCpuUsage = "k8s.node.cpu.usage";
static constexpr const char *descrMetricK8sNodeCpuUsage =
    "Node's CPU usage, measured in cpus. Range from 0 to the number of allocatable CPUs";
static constexpr const char *unitMetricK8sNodeCpuUsage = "{cpu}";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricK8sNodeCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricK8sNodeCpuUsage, descrMetricK8sNodeCpuUsage,
                                 unitMetricK8sNodeCpuUsage);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricK8sNodeCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricK8sNodeCpuUsage, descrMetricK8sNodeCpuUsage,
                                  unitMetricK8sNodeCpuUsage);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sNodeCpuUsage(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricK8sNodeCpuUsage, descrMetricK8sNodeCpuUsage,
                                           unitMetricK8sNodeCpuUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sNodeCpuUsage(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricK8sNodeCpuUsage, descrMetricK8sNodeCpuUsage,
                                            unitMetricK8sNodeCpuUsage);
}

/**
 * Memory usage of the Node
 * <p>
 * Total memory usage of the Node
 * <p>
 * gauge
 */
static constexpr const char *kMetricK8sNodeMemoryUsage     = "k8s.node.memory.usage";
static constexpr const char *descrMetricK8sNodeMemoryUsage = "Memory usage of the Node";
static constexpr const char *unitMetricK8sNodeMemoryUsage  = "By";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricK8sNodeMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricK8sNodeMemoryUsage, descrMetricK8sNodeMemoryUsage,
                                 unitMetricK8sNodeMemoryUsage);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricK8sNodeMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricK8sNodeMemoryUsage, descrMetricK8sNodeMemoryUsage,
                                  unitMetricK8sNodeMemoryUsage);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sNodeMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricK8sNodeMemoryUsage, descrMetricK8sNodeMemoryUsage,
                                           unitMetricK8sNodeMemoryUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sNodeMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(
      kMetricK8sNodeMemoryUsage, descrMetricK8sNodeMemoryUsage, unitMetricK8sNodeMemoryUsage);
}

/**
 * Node network errors
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sNodeNetworkErrors     = "k8s.node.network.errors";
static constexpr const char *descrMetricK8sNodeNetworkErrors = "Node network errors";
static constexpr const char *unitMetricK8sNodeNetworkErrors  = "{error}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncInt64MetricK8sNodeNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricK8sNodeNetworkErrors, descrMetricK8sNodeNetworkErrors,
                                    unitMetricK8sNodeNetworkErrors);
}

static inline nostd::unique_ptr<metrics::Counter<double>>
CreateSyncDoubleMetricK8sNodeNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricK8sNodeNetworkErrors, descrMetricK8sNodeNetworkErrors,
                                    unitMetricK8sNodeNetworkErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sNodeNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricK8sNodeNetworkErrors, descrMetricK8sNodeNetworkErrors, unitMetricK8sNodeNetworkErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sNodeNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(
      kMetricK8sNodeNetworkErrors, descrMetricK8sNodeNetworkErrors, unitMetricK8sNodeNetworkErrors);
}

/**
 * Network bytes for the Node
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sNodeNetworkIo     = "k8s.node.network.io";
static constexpr const char *descrMetricK8sNodeNetworkIo = "Network bytes for the Node";
static constexpr const char *unitMetricK8sNodeNetworkIo  = "By";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricK8sNodeNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricK8sNodeNetworkIo, descrMetricK8sNodeNetworkIo,
                                    unitMetricK8sNodeNetworkIo);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricK8sNodeNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricK8sNodeNetworkIo, descrMetricK8sNodeNetworkIo,
                                    unitMetricK8sNodeNetworkIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sNodeNetworkIo(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricK8sNodeNetworkIo, descrMetricK8sNodeNetworkIo,
                                             unitMetricK8sNodeNetworkIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sNodeNetworkIo(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricK8sNodeNetworkIo, descrMetricK8sNodeNetworkIo,
                                              unitMetricK8sNodeNetworkIo);
}

/**
 * The time the Node has been running
 * <p>
 * Instrumentations SHOULD use a gauge with type @code double @endcode and measure uptime in seconds
 * as a floating point number with the highest precision available. The actual accuracy would depend
 * on the instrumentation and operating system. <p> gauge
 */
static constexpr const char *kMetricK8sNodeUptime     = "k8s.node.uptime";
static constexpr const char *descrMetricK8sNodeUptime = "The time the Node has been running";
static constexpr const char *unitMetricK8sNodeUptime  = "s";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricK8sNodeUptime(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricK8sNodeUptime, descrMetricK8sNodeUptime,
                                 unitMetricK8sNodeUptime);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricK8sNodeUptime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricK8sNodeUptime, descrMetricK8sNodeUptime,
                                  unitMetricK8sNodeUptime);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricK8sNodeUptime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricK8sNodeUptime, descrMetricK8sNodeUptime,
                                           unitMetricK8sNodeUptime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricK8sNodeUptime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricK8sNodeUptime, descrMetricK8sNodeUptime,
                                            unitMetricK8sNodeUptime);
}

/**
 * Total CPU time consumed
 * <p>
 * Total CPU time consumed by the specific Pod on all available CPU cores
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sPodCpuTime     = "k8s.pod.cpu.time";
static constexpr const char *descrMetricK8sPodCpuTime = "Total CPU time consumed";
static constexpr const char *unitMetricK8sPodCpuTime  = "s";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricK8sPodCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricK8sPodCpuTime, descrMetricK8sPodCpuTime,
                                    unitMetricK8sPodCpuTime);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricK8sPodCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricK8sPodCpuTime, descrMetricK8sPodCpuTime,
                                    unitMetricK8sPodCpuTime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricK8sPodCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricK8sPodCpuTime, descrMetricK8sPodCpuTime,
                                             unitMetricK8sPodCpuTime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricK8sPodCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricK8sPodCpuTime, descrMetricK8sPodCpuTime,
                                              unitMetricK8sPodCpuTime);
}

/**
 * Pod's CPU usage, measured in cpus. Range from 0 to the number of allocatable CPUs
 * <p>
 * CPU usage of the specific Pod on all available CPU cores, averaged over the sample window
 * <p>
 * gauge
 */
static constexpr const char *kMetricK8sPodCpuUsage = "k8s.pod.cpu.usage";
static constexpr const char *descrMetricK8sPodCpuUsage =
    "Pod's CPU usage, measured in cpus. Range from 0 to the number of allocatable CPUs";
static constexpr const char *unitMetricK8sPodCpuUsage = "{cpu}";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricK8sPodCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricK8sPodCpuUsage, descrMetricK8sPodCpuUsage,
                                 unitMetricK8sPodCpuUsage);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricK8sPodCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricK8sPodCpuUsage, descrMetricK8sPodCpuUsage,
                                  unitMetricK8sPodCpuUsage);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricK8sPodCpuUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricK8sPodCpuUsage, descrMetricK8sPodCpuUsage,
                                           unitMetricK8sPodCpuUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sPodCpuUsage(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricK8sPodCpuUsage, descrMetricK8sPodCpuUsage,
                                            unitMetricK8sPodCpuUsage);
}

/**
 * Memory usage of the Pod
 * <p>
 * Total memory usage of the Pod
 * <p>
 * gauge
 */
static constexpr const char *kMetricK8sPodMemoryUsage     = "k8s.pod.memory.usage";
static constexpr const char *descrMetricK8sPodMemoryUsage = "Memory usage of the Pod";
static constexpr const char *unitMetricK8sPodMemoryUsage  = "By";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricK8sPodMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricK8sPodMemoryUsage, descrMetricK8sPodMemoryUsage,
                                 unitMetricK8sPodMemoryUsage);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricK8sPodMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricK8sPodMemoryUsage, descrMetricK8sPodMemoryUsage,
                                  unitMetricK8sPodMemoryUsage);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sPodMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricK8sPodMemoryUsage, descrMetricK8sPodMemoryUsage,
                                           unitMetricK8sPodMemoryUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sPodMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricK8sPodMemoryUsage, descrMetricK8sPodMemoryUsage,
                                            unitMetricK8sPodMemoryUsage);
}

/**
 * Pod network errors
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sPodNetworkErrors     = "k8s.pod.network.errors";
static constexpr const char *descrMetricK8sPodNetworkErrors = "Pod network errors";
static constexpr const char *unitMetricK8sPodNetworkErrors  = "{error}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncInt64MetricK8sPodNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricK8sPodNetworkErrors, descrMetricK8sPodNetworkErrors,
                                    unitMetricK8sPodNetworkErrors);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricK8sPodNetworkErrors(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricK8sPodNetworkErrors, descrMetricK8sPodNetworkErrors,
                                    unitMetricK8sPodNetworkErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sPodNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricK8sPodNetworkErrors, descrMetricK8sPodNetworkErrors, unitMetricK8sPodNetworkErrors);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sPodNetworkErrors(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(
      kMetricK8sPodNetworkErrors, descrMetricK8sPodNetworkErrors, unitMetricK8sPodNetworkErrors);
}

/**
 * Network bytes for the Pod
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sPodNetworkIo     = "k8s.pod.network.io";
static constexpr const char *descrMetricK8sPodNetworkIo = "Network bytes for the Pod";
static constexpr const char *unitMetricK8sPodNetworkIo  = "By";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricK8sPodNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricK8sPodNetworkIo, descrMetricK8sPodNetworkIo,
                                    unitMetricK8sPodNetworkIo);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricK8sPodNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricK8sPodNetworkIo, descrMetricK8sPodNetworkIo,
                                    unitMetricK8sPodNetworkIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricK8sPodNetworkIo(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricK8sPodNetworkIo, descrMetricK8sPodNetworkIo,
                                             unitMetricK8sPodNetworkIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricK8sPodNetworkIo(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricK8sPodNetworkIo, descrMetricK8sPodNetworkIo,
                                              unitMetricK8sPodNetworkIo);
}

/**
 * The time the Pod has been running
 * <p>
 * Instrumentations SHOULD use a gauge with type @code double @endcode and measure uptime in seconds
 * as a floating point number with the highest precision available. The actual accuracy would depend
 * on the instrumentation and operating system. <p> gauge
 */
static constexpr const char *kMetricK8sPodUptime     = "k8s.pod.uptime";
static constexpr const char *descrMetricK8sPodUptime = "The time the Pod has been running";
static constexpr const char *unitMetricK8sPodUptime  = "s";

#if OPENTELEMETRY_ABI_VERSION_NO >= 2

static inline nostd::unique_ptr<metrics::Gauge<int64_t>> CreateSyncInt64MetricK8sPodUptime(
    metrics::Meter *meter)
{
  return meter->CreateInt64Gauge(kMetricK8sPodUptime, descrMetricK8sPodUptime,
                                 unitMetricK8sPodUptime);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricK8sPodUptime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricK8sPodUptime, descrMetricK8sPodUptime,
                                  unitMetricK8sPodUptime);
}
#endif /* OPENTELEMETRY_ABI_VERSION_NO */

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricK8sPodUptime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricK8sPodUptime, descrMetricK8sPodUptime,
                                           unitMetricK8sPodUptime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricK8sPodUptime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricK8sPodUptime, descrMetricK8sPodUptime,
                                            unitMetricK8sPodUptime);
}

}  // namespace k8s
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
