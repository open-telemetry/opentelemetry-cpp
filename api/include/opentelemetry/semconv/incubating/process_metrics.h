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
namespace process
{

/**
 * Number of times the process has been context switched.
 * <p>
 * counter
 */
static constexpr const char *kMetricProcessContextSwitches = "metric.process.context_switches";
static constexpr const char *descrMetricProcessContextSwitches =
    "Number of times the process has been context switched.";
static constexpr const char *unitMetricProcessContextSwitches = "{count}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncInt64MetricProcessContextSwitches(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessContextSwitches,
                                    descrMetricProcessContextSwitches,
                                    unitMetricProcessContextSwitches);
}

static inline nostd::unique_ptr<metrics::Counter<double>>
CreateSyncDoubleMetricProcessContextSwitches(metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricProcessContextSwitches,
                                    descrMetricProcessContextSwitches,
                                    unitMetricProcessContextSwitches);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessContextSwitches(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessContextSwitches,
                                             descrMetricProcessContextSwitches,
                                             unitMetricProcessContextSwitches);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessContextSwitches(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricProcessContextSwitches,
                                              descrMetricProcessContextSwitches,
                                              unitMetricProcessContextSwitches);
}

/**
 * Total CPU seconds broken down by different states.
 * <p>
 * counter
 */
static constexpr const char *kMetricProcessCpuTime = "metric.process.cpu.time";
static constexpr const char *descrMetricProcessCpuTime =
    "Total CPU seconds broken down by different states.";
static constexpr const char *unitMetricProcessCpuTime = "s";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricProcessCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessCpuTime, descrMetricProcessCpuTime,
                                    unitMetricProcessCpuTime);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricProcessCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricProcessCpuTime, descrMetricProcessCpuTime,
                                    unitMetricProcessCpuTime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricProcessCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessCpuTime, descrMetricProcessCpuTime,
                                             unitMetricProcessCpuTime);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessCpuTime(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricProcessCpuTime, descrMetricProcessCpuTime,
                                              unitMetricProcessCpuTime);
}

/**
 * Difference in process.cpu.time since the last measurement, divided by the elapsed time and number
 * of CPUs available to the process. <p> gauge
 */
static constexpr const char *kMetricProcessCpuUtilization = "metric.process.cpu.utilization";
static constexpr const char *descrMetricProcessCpuUtilization =
    "Difference in process.cpu.time since the last measurement, divided by the elapsed time and "
    "number of CPUs available to the process.";
static constexpr const char *unitMetricProcessCpuUtilization = "1";

#ifdef OPENTELEMETRY_LATER
// Unsupported: Sync gauge
static inline nostd::unique_ptr<metrics::Gauge<uint64_t>>
CreateSyncInt64MetricProcessCpuUtilization(metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricProcessCpuUtilization, descrMetricProcessCpuUtilization,
                                  unitMetricProcessCpuUtilization);
}

static inline nostd::unique_ptr<metrics::Gauge<double>> CreateSyncDoubleMetricProcessCpuUtilization(
    metrics::Meter *meter)
{
  return meter->CreateDoubleGauge(kMetricProcessCpuUtilization, descrMetricProcessCpuUtilization,
                                  unitMetricProcessCpuUtilization);
}
#endif /* OPENTELEMETRY_LATER */

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessCpuUtilization(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricProcessCpuUtilization,
                                           descrMetricProcessCpuUtilization,
                                           unitMetricProcessCpuUtilization);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessCpuUtilization(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableGauge(kMetricProcessCpuUtilization,
                                            descrMetricProcessCpuUtilization,
                                            unitMetricProcessCpuUtilization);
}

/**
 * Disk bytes transferred.
 * <p>
 * counter
 */
static constexpr const char *kMetricProcessDiskIo     = "metric.process.disk.io";
static constexpr const char *descrMetricProcessDiskIo = "Disk bytes transferred.";
static constexpr const char *unitMetricProcessDiskIo  = "By";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricProcessDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessDiskIo, descrMetricProcessDiskIo,
                                    unitMetricProcessDiskIo);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricProcessDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricProcessDiskIo, descrMetricProcessDiskIo,
                                    unitMetricProcessDiskIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncInt64MetricProcessDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessDiskIo, descrMetricProcessDiskIo,
                                             unitMetricProcessDiskIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncDoubleMetricProcessDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricProcessDiskIo, descrMetricProcessDiskIo,
                                              unitMetricProcessDiskIo);
}

/**
 * The amount of physical memory in use.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricProcessMemoryUsage = "metric.process.memory.usage";
static constexpr const char *descrMetricProcessMemoryUsage =
    "The amount of physical memory in use.";
static constexpr const char *unitMetricProcessMemoryUsage = "By";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricProcessMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricProcessMemoryUsage, descrMetricProcessMemoryUsage,
                                         unitMetricProcessMemoryUsage);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricProcessMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricProcessMemoryUsage, descrMetricProcessMemoryUsage,
                                          unitMetricProcessMemoryUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricProcessMemoryUsage, descrMetricProcessMemoryUsage, unitMetricProcessMemoryUsage);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessMemoryUsage(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(
      kMetricProcessMemoryUsage, descrMetricProcessMemoryUsage, unitMetricProcessMemoryUsage);
}

/**
 * The amount of committed virtual memory.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricProcessMemoryVirtual = "metric.process.memory.virtual";
static constexpr const char *descrMetricProcessMemoryVirtual =
    "The amount of committed virtual memory.";
static constexpr const char *unitMetricProcessMemoryVirtual = "By";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricProcessMemoryVirtual(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(
      kMetricProcessMemoryVirtual, descrMetricProcessMemoryVirtual, unitMetricProcessMemoryVirtual);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricProcessMemoryVirtual(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(
      kMetricProcessMemoryVirtual, descrMetricProcessMemoryVirtual, unitMetricProcessMemoryVirtual);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessMemoryVirtual(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricProcessMemoryVirtual, descrMetricProcessMemoryVirtual, unitMetricProcessMemoryVirtual);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessMemoryVirtual(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(
      kMetricProcessMemoryVirtual, descrMetricProcessMemoryVirtual, unitMetricProcessMemoryVirtual);
}

/**
 * Network bytes transferred.
 * <p>
 * counter
 */
static constexpr const char *kMetricProcessNetworkIo     = "metric.process.network.io";
static constexpr const char *descrMetricProcessNetworkIo = "Network bytes transferred.";
static constexpr const char *unitMetricProcessNetworkIo  = "By";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncInt64MetricProcessNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessNetworkIo, descrMetricProcessNetworkIo,
                                    unitMetricProcessNetworkIo);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricProcessNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricProcessNetworkIo, descrMetricProcessNetworkIo,
                                    unitMetricProcessNetworkIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessNetworkIo(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessNetworkIo, descrMetricProcessNetworkIo,
                                             unitMetricProcessNetworkIo);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessNetworkIo(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(kMetricProcessNetworkIo, descrMetricProcessNetworkIo,
                                              unitMetricProcessNetworkIo);
}

/**
 * Number of file descriptors in use by the process.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricProcessOpenFileDescriptorCount =
    "metric.process.open_file_descriptor.count";
static constexpr const char *descrMetricProcessOpenFileDescriptorCount =
    "Number of file descriptors in use by the process.";
static constexpr const char *unitMetricProcessOpenFileDescriptorCount = "{count}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricProcessOpenFileDescriptorCount(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricProcessOpenFileDescriptorCount,
                                         descrMetricProcessOpenFileDescriptorCount,
                                         unitMetricProcessOpenFileDescriptorCount);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricProcessOpenFileDescriptorCount(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricProcessOpenFileDescriptorCount,
                                          descrMetricProcessOpenFileDescriptorCount,
                                          unitMetricProcessOpenFileDescriptorCount);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessOpenFileDescriptorCount(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricProcessOpenFileDescriptorCount,
                                                   descrMetricProcessOpenFileDescriptorCount,
                                                   unitMetricProcessOpenFileDescriptorCount);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessOpenFileDescriptorCount(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(kMetricProcessOpenFileDescriptorCount,
                                                    descrMetricProcessOpenFileDescriptorCount,
                                                    unitMetricProcessOpenFileDescriptorCount);
}

/**
 * Number of page faults the process has made.
 * <p>
 * counter
 */
static constexpr const char *kMetricProcessPagingFaults = "metric.process.paging.faults";
static constexpr const char *descrMetricProcessPagingFaults =
    "Number of page faults the process has made.";
static constexpr const char *unitMetricProcessPagingFaults = "{fault}";

static inline nostd::unique_ptr<metrics::Counter<uint64_t>>
CreateSyncInt64MetricProcessPagingFaults(metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessPagingFaults, descrMetricProcessPagingFaults,
                                    unitMetricProcessPagingFaults);
}

static inline nostd::unique_ptr<metrics::Counter<double>> CreateSyncDoubleMetricProcessPagingFaults(
    metrics::Meter *meter)
{
  return meter->CreateDoubleCounter(kMetricProcessPagingFaults, descrMetricProcessPagingFaults,
                                    unitMetricProcessPagingFaults);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessPagingFaults(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricProcessPagingFaults, descrMetricProcessPagingFaults, unitMetricProcessPagingFaults);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessPagingFaults(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableCounter(
      kMetricProcessPagingFaults, descrMetricProcessPagingFaults, unitMetricProcessPagingFaults);
}

/**
 * Process threads count.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricProcessThreadCount     = "metric.process.thread.count";
static constexpr const char *descrMetricProcessThreadCount = "Process threads count.";
static constexpr const char *unitMetricProcessThreadCount  = "{thread}";

static inline nostd::unique_ptr<metrics::UpDownCounter<int64_t>>
CreateSyncInt64MetricProcessThreadCount(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricProcessThreadCount, descrMetricProcessThreadCount,
                                         unitMetricProcessThreadCount);
}

static inline nostd::unique_ptr<metrics::UpDownCounter<double>>
CreateSyncDoubleMetricProcessThreadCount(metrics::Meter *meter)
{
  return meter->CreateDoubleUpDownCounter(kMetricProcessThreadCount, descrMetricProcessThreadCount,
                                          unitMetricProcessThreadCount);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncInt64MetricProcessThreadCount(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricProcessThreadCount, descrMetricProcessThreadCount, unitMetricProcessThreadCount);
}

static inline nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncDoubleMetricProcessThreadCount(metrics::Meter *meter)
{
  return meter->CreateDoubleObservableUpDownCounter(
      kMetricProcessThreadCount, descrMetricProcessThreadCount, unitMetricProcessThreadCount);
}

}  // namespace process
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
