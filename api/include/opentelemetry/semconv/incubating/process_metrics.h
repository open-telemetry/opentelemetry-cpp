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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricProcessContextSwitches(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessContextSwitches,
                                    descrMetricProcessContextSwitches,
                                    unitMetricProcessContextSwitches);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessContextSwitches(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessContextSwitches,
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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricProcessCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessCpuTime, descrMetricProcessCpuTime,
                                    unitMetricProcessCpuTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessCpuTime, descrMetricProcessCpuTime,
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

static nostd::unique_ptr<metrics::Gauge<uint64_t>> CreateSyncMetricProcessCpuUtilization(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricProcessCpuUtilization, descrMetricProcessCpuUtilization,
                                  unitMetricProcessCpuUtilization);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessCpuUtilization(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricProcessCpuUtilization,
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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricProcessDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessDiskIo, descrMetricProcessDiskIo,
                                    unitMetricProcessDiskIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessDiskIo, descrMetricProcessDiskIo,
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricProcessMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricProcessMemoryUsage, descrMetricProcessMemoryUsage,
                                         unitMetricProcessMemoryUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricProcessMemoryVirtual(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(
      kMetricProcessMemoryVirtual, descrMetricProcessMemoryVirtual, unitMetricProcessMemoryVirtual);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessMemoryVirtual(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricProcessNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessNetworkIo, descrMetricProcessNetworkIo,
                                    unitMetricProcessNetworkIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricProcessNetworkIo, descrMetricProcessNetworkIo,
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricProcessOpenFileDescriptorCount(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricProcessOpenFileDescriptorCount,
                                         descrMetricProcessOpenFileDescriptorCount,
                                         unitMetricProcessOpenFileDescriptorCount);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricProcessOpenFileDescriptorCount(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricProcessOpenFileDescriptorCount,
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

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricProcessPagingFaults(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricProcessPagingFaults, descrMetricProcessPagingFaults,
                                    unitMetricProcessPagingFaults);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessPagingFaults(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
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

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricProcessThreadCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricProcessThreadCount, descrMetricProcessThreadCount,
                                         unitMetricProcessThreadCount);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricProcessThreadCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricProcessThreadCount, descrMetricProcessThreadCount, unitMetricProcessThreadCount);
}

}  // namespace process
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
