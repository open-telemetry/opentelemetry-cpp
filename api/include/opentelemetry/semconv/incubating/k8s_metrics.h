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
static constexpr const char *kMetricK8sNodeCpuTime     = "metric.k8s.node.cpu.time";
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
static constexpr const char *kMetricK8sNodeCpuUsage = "metric.k8s.node.cpu.usage";
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
static constexpr const char *kMetricK8sNodeMemoryUsage     = "metric.k8s.node.memory.usage";
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
 * Total CPU time consumed
 * <p>
 * Total CPU time consumed by the specific Pod on all available CPU cores
 * <p>
 * counter
 */
static constexpr const char *kMetricK8sPodCpuTime     = "metric.k8s.pod.cpu.time";
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
static constexpr const char *kMetricK8sPodCpuUsage = "metric.k8s.pod.cpu.usage";
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
static constexpr const char *kMetricK8sPodMemoryUsage     = "metric.k8s.pod.memory.usage";
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

}  // namespace k8s
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
