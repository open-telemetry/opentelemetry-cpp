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
namespace system
{

/**
 * Reports the current frequency of the CPU in Hz
 * <p>
 * gauge
 */
static constexpr const char *kMetricSystemCpuFrequency = "metric.system.cpu.frequency";
static constexpr const char *descrMetricSystemCpuFrequency =
    "Reports the current frequency of the CPU in Hz";
static constexpr const char *unitMetricSystemCpuFrequency = "{Hz}";

static nostd::unique_ptr<metrics::Gauge<uint64_t>> CreateSyncMetricSystemCpuFrequency(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricSystemCpuFrequency, descrMetricSystemCpuFrequency,
                                  unitMetricSystemCpuFrequency);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemCpuFrequency(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricSystemCpuFrequency, descrMetricSystemCpuFrequency,
                                           unitMetricSystemCpuFrequency);
}

/**
 * Reports the number of logical (virtual) processor cores created by the operating system to manage
 * multitasking <p> updowncounter
 */
static constexpr const char *kMetricSystemCpuLogicalCount = "metric.system.cpu.logical.count";
static constexpr const char *descrMetricSystemCpuLogicalCount =
    "Reports the number of logical (virtual) processor cores created by the operating system to "
    "manage multitasking";
static constexpr const char *unitMetricSystemCpuLogicalCount = "{cpu}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemCpuLogicalCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemCpuLogicalCount,
                                         descrMetricSystemCpuLogicalCount,
                                         unitMetricSystemCpuLogicalCount);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemCpuLogicalCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricSystemCpuLogicalCount,
                                                   descrMetricSystemCpuLogicalCount,
                                                   unitMetricSystemCpuLogicalCount);
}

/**
 * Reports the number of actual physical processor cores on the hardware
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricSystemCpuPhysicalCount = "metric.system.cpu.physical.count";
static constexpr const char *descrMetricSystemCpuPhysicalCount =
    "Reports the number of actual physical processor cores on the hardware";
static constexpr const char *unitMetricSystemCpuPhysicalCount = "{cpu}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemCpuPhysicalCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemCpuPhysicalCount,
                                         descrMetricSystemCpuPhysicalCount,
                                         unitMetricSystemCpuPhysicalCount);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemCpuPhysicalCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricSystemCpuPhysicalCount,
                                                   descrMetricSystemCpuPhysicalCount,
                                                   unitMetricSystemCpuPhysicalCount);
}

/**
 * Seconds each logical CPU spent on each mode
 * <p>
 * counter
 */
static constexpr const char *kMetricSystemCpuTime = "metric.system.cpu.time";
static constexpr const char *descrMetricSystemCpuTime =
    "Seconds each logical CPU spent on each mode";
static constexpr const char *unitMetricSystemCpuTime = "s";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemCpuTime, descrMetricSystemCpuTime,
                                    unitMetricSystemCpuTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemCpuTime, descrMetricSystemCpuTime,
                                             unitMetricSystemCpuTime);
}

/**
 * Difference in system.cpu.time since the last measurement, divided by the elapsed time and number
 * of logical CPUs <p> gauge
 */
static constexpr const char *kMetricSystemCpuUtilization = "metric.system.cpu.utilization";
static constexpr const char *descrMetricSystemCpuUtilization =
    "Difference in system.cpu.time since the last measurement, divided by the elapsed time and "
    "number of logical CPUs";
static constexpr const char *unitMetricSystemCpuUtilization = "1";

static nostd::unique_ptr<metrics::Gauge<uint64_t>> CreateSyncMetricSystemCpuUtilization(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricSystemCpuUtilization, descrMetricSystemCpuUtilization,
                                  unitMetricSystemCpuUtilization);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemCpuUtilization(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(
      kMetricSystemCpuUtilization, descrMetricSystemCpuUtilization, unitMetricSystemCpuUtilization);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemDiskIo     = "metric.system.disk.io";
static constexpr const char *descrMetricSystemDiskIo = "";
static constexpr const char *unitMetricSystemDiskIo  = "By";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemDiskIo, descrMetricSystemDiskIo,
                                    unitMetricSystemDiskIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemDiskIo, descrMetricSystemDiskIo,
                                             unitMetricSystemDiskIo);
}

/**
 * Time disk spent activated
 * <p>
 * The real elapsed time ("wall clock") used in the I/O path (time from operations running in
 * parallel are not counted). Measured as: <p> <ul> <li>Linux: Field 13 from <a
 * href="https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats">procfs-diskstats</a></li>
 *   <li>Windows: The complement of
 * <a
 * href="https://learn.microsoft.com/archive/blogs/askcore/windows-performance-monitor-disk-counters-explained#windows-performance-monitor-disk-counters-explained">"Disk%
 * Idle Time"</a> performance counter: @code uptime * (100 - "Disk\% Idle Time") / 100 @endcode</li>
 * </ul>
 * counter
 */
static constexpr const char *kMetricSystemDiskIoTime     = "metric.system.disk.io_time";
static constexpr const char *descrMetricSystemDiskIoTime = "Time disk spent activated";
static constexpr const char *unitMetricSystemDiskIoTime  = "s";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemDiskIoTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemDiskIoTime, descrMetricSystemDiskIoTime,
                                    unitMetricSystemDiskIoTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemDiskIoTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemDiskIoTime, descrMetricSystemDiskIoTime,
                                             unitMetricSystemDiskIoTime);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemDiskMerged     = "metric.system.disk.merged";
static constexpr const char *descrMetricSystemDiskMerged = "";
static constexpr const char *unitMetricSystemDiskMerged  = "{operation}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemDiskMerged(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemDiskMerged, descrMetricSystemDiskMerged,
                                    unitMetricSystemDiskMerged);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemDiskMerged(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemDiskMerged, descrMetricSystemDiskMerged,
                                             unitMetricSystemDiskMerged);
}

/**
 * Sum of the time each operation took to complete
 * <p>
 * Because it is the sum of time each request took, parallel-issued requests each contribute to make
 * the count grow. Measured as: <p> <ul> <li>Linux: Fields 7 & 11 from <a
 * href="https://www.kernel.org/doc/Documentation/ABI/testing/procfs-diskstats">procfs-diskstats</a></li>
 *   <li>Windows: "Avg. Disk sec/Read" perf counter multiplied by "Disk Reads/sec" perf counter
 * (similar for Writes)</li>
 * </ul>
 * counter
 */
static constexpr const char *kMetricSystemDiskOperationTime = "metric.system.disk.operation_time";
static constexpr const char *descrMetricSystemDiskOperationTime =
    "Sum of the time each operation took to complete";
static constexpr const char *unitMetricSystemDiskOperationTime = "s";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemDiskOperationTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemDiskOperationTime,
                                    descrMetricSystemDiskOperationTime,
                                    unitMetricSystemDiskOperationTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemDiskOperationTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemDiskOperationTime,
                                             descrMetricSystemDiskOperationTime,
                                             unitMetricSystemDiskOperationTime);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemDiskOperations     = "metric.system.disk.operations";
static constexpr const char *descrMetricSystemDiskOperations = "";
static constexpr const char *unitMetricSystemDiskOperations  = "{operation}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemDiskOperations(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemDiskOperations, descrMetricSystemDiskOperations,
                                    unitMetricSystemDiskOperations);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemDiskOperations(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricSystemDiskOperations, descrMetricSystemDiskOperations, unitMetricSystemDiskOperations);
}

/**
 * updowncounter
 */
static constexpr const char *kMetricSystemFilesystemUsage     = "metric.system.filesystem.usage";
static constexpr const char *descrMetricSystemFilesystemUsage = "";
static constexpr const char *unitMetricSystemFilesystemUsage  = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemFilesystemUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemFilesystemUsage,
                                         descrMetricSystemFilesystemUsage,
                                         unitMetricSystemFilesystemUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemFilesystemUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricSystemFilesystemUsage,
                                                   descrMetricSystemFilesystemUsage,
                                                   unitMetricSystemFilesystemUsage);
}

/**
 * gauge
 */
static constexpr const char *kMetricSystemFilesystemUtilization =
    "metric.system.filesystem.utilization";
static constexpr const char *descrMetricSystemFilesystemUtilization = "";
static constexpr const char *unitMetricSystemFilesystemUtilization  = "1";

static nostd::unique_ptr<metrics::Gauge<uint64_t>> CreateSyncMetricSystemFilesystemUtilization(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricSystemFilesystemUtilization,
                                  descrMetricSystemFilesystemUtilization,
                                  unitMetricSystemFilesystemUtilization);
}

static nostd::shared_ptr<metrics::ObservableInstrument>
CreateAsyncMetricSystemFilesystemUtilization(metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricSystemFilesystemUtilization,
                                           descrMetricSystemFilesystemUtilization,
                                           unitMetricSystemFilesystemUtilization);
}

/**
 * An estimate of how much memory is available for starting new applications, without causing
 * swapping <p> This is an alternative to @code system.memory.usage @endcode metric with @code
 * state=free @endcode. Linux starting from 3.14 exports "available" memory. It takes "free" memory
 * as a baseline, and then factors in kernel-specific values. This is supposed to be more accurate
 * than just "free" memory. For reference, see the calculations <a
 * href="https://superuser.com/a/980821">here</a>. See also @code MemAvailable @endcode in <a
 * href="https://man7.org/linux/man-pages/man5/proc.5.html">/proc/meminfo</a>. <p> updowncounter
 */
static constexpr const char *kMetricSystemLinuxMemoryAvailable =
    "metric.system.linux.memory.available";
static constexpr const char *descrMetricSystemLinuxMemoryAvailable =
    "An estimate of how much memory is available for starting new applications, without causing "
    "swapping";
static constexpr const char *unitMetricSystemLinuxMemoryAvailable = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricSystemLinuxMemoryAvailable(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemLinuxMemoryAvailable,
                                         descrMetricSystemLinuxMemoryAvailable,
                                         unitMetricSystemLinuxMemoryAvailable);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemLinuxMemoryAvailable(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricSystemLinuxMemoryAvailable,
                                                   descrMetricSystemLinuxMemoryAvailable,
                                                   unitMetricSystemLinuxMemoryAvailable);
}

/**
 * Reports the memory used by the Linux kernel for managing caches of frequently used objects.
 * <p>
 * The sum over the @code reclaimable @endcode and @code unreclaimable @endcode state values in
 * @code linux.memory.slab.usage @endcode SHOULD be equal to the total slab memory available on the
 * system. Note that the total slab memory is not constant and may vary over time. See also the <a
 * href="https://blogs.oracle.com/linux/post/understanding-linux-kernel-memory-statistics">Slab
 * allocator</a> and @code Slab @endcode in <a
 * href="https://man7.org/linux/man-pages/man5/proc.5.html">/proc/meminfo</a>. <p> updowncounter
 */
static constexpr const char *kMetricSystemLinuxMemorySlabUsage =
    "metric.system.linux.memory.slab.usage";
static constexpr const char *descrMetricSystemLinuxMemorySlabUsage =
    "Reports the memory used by the Linux kernel for managing caches of frequently used objects.";
static constexpr const char *unitMetricSystemLinuxMemorySlabUsage = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>>
CreateSyncMetricSystemLinuxMemorySlabUsage(metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemLinuxMemorySlabUsage,
                                         descrMetricSystemLinuxMemorySlabUsage,
                                         unitMetricSystemLinuxMemorySlabUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemLinuxMemorySlabUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricSystemLinuxMemorySlabUsage,
                                                   descrMetricSystemLinuxMemorySlabUsage,
                                                   unitMetricSystemLinuxMemorySlabUsage);
}

/**
 * Total memory available in the system.
 * <p>
 * Its value SHOULD equal the sum of @code system.memory.state @endcode over all states.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricSystemMemoryLimit     = "metric.system.memory.limit";
static constexpr const char *descrMetricSystemMemoryLimit = "Total memory available in the system.";
static constexpr const char *unitMetricSystemMemoryLimit  = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemMemoryLimit(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemMemoryLimit, descrMetricSystemMemoryLimit,
                                         unitMetricSystemMemoryLimit);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemMemoryLimit(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricSystemMemoryLimit, descrMetricSystemMemoryLimit, unitMetricSystemMemoryLimit);
}

/**
 * Shared memory used (mostly by tmpfs).
 * <p>
 * Equivalent of @code shared @endcode from <a
 * href="https://man7.org/linux/man-pages/man1/free.1.html">@code free @endcode command</a> or
 * @code Shmem @endcode from <a href="https://man7.org/linux/man-pages/man5/proc.5.html">@code
 * /proc/meminfo @endcode</a>" <p> updowncounter
 */
static constexpr const char *kMetricSystemMemoryShared = "metric.system.memory.shared";
static constexpr const char *descrMetricSystemMemoryShared =
    "Shared memory used (mostly by tmpfs).";
static constexpr const char *unitMetricSystemMemoryShared = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemMemoryShared(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemMemoryShared, descrMetricSystemMemoryShared,
                                         unitMetricSystemMemoryShared);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemMemoryShared(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricSystemMemoryShared, descrMetricSystemMemoryShared, unitMetricSystemMemoryShared);
}

/**
 * Reports memory in use by state.
 * <p>
 * The sum over all @code system.memory.state @endcode values SHOULD equal the total memory
 * available on the system, that is @code system.memory.limit @endcode.
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricSystemMemoryUsage     = "metric.system.memory.usage";
static constexpr const char *descrMetricSystemMemoryUsage = "Reports memory in use by state.";
static constexpr const char *unitMetricSystemMemoryUsage  = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemMemoryUsage, descrMetricSystemMemoryUsage,
                                         unitMetricSystemMemoryUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricSystemMemoryUsage, descrMetricSystemMemoryUsage, unitMetricSystemMemoryUsage);
}

/**
 * gauge
 */
static constexpr const char *kMetricSystemMemoryUtilization = "metric.system.memory.utilization";
static constexpr const char *descrMetricSystemMemoryUtilization = "";
static constexpr const char *unitMetricSystemMemoryUtilization  = "1";

static nostd::unique_ptr<metrics::Gauge<uint64_t>> CreateSyncMetricSystemMemoryUtilization(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricSystemMemoryUtilization,
                                  descrMetricSystemMemoryUtilization,
                                  unitMetricSystemMemoryUtilization);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemMemoryUtilization(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricSystemMemoryUtilization,
                                           descrMetricSystemMemoryUtilization,
                                           unitMetricSystemMemoryUtilization);
}

/**
 * updowncounter
 */
static constexpr const char *kMetricSystemNetworkConnections = "metric.system.network.connections";
static constexpr const char *descrMetricSystemNetworkConnections = "";
static constexpr const char *unitMetricSystemNetworkConnections  = "{connection}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemNetworkConnections(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemNetworkConnections,
                                         descrMetricSystemNetworkConnections,
                                         unitMetricSystemNetworkConnections);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemNetworkConnections(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(kMetricSystemNetworkConnections,
                                                   descrMetricSystemNetworkConnections,
                                                   unitMetricSystemNetworkConnections);
}

/**
 * Count of packets that are dropped or discarded even though there was no error
 * <p>
 * Measured as:
 * <p>
 * <ul>
 *   <li>Linux: the @code drop @endcode column in @code /proc/dev/net @endcode (<a
 * href="https://web.archive.org/web/20180321091318/http://www.onlamp.com/pub/a/linux/2000/11/16/LinuxAdmin.html">source</a>)</li>
 *   <li>Windows: <a
 * href="https://docs.microsoft.com/windows/win32/api/netioapi/ns-netioapi-mib_if_row2">@code
 * InDiscards @endcode/@code OutDiscards @endcode</a> from <a
 * href="https://docs.microsoft.com/windows/win32/api/netioapi/nf-netioapi-getifentry2">@code
 * GetIfEntry2 @endcode</a></li>
 * </ul>
 * counter
 */
static constexpr const char *kMetricSystemNetworkDropped = "metric.system.network.dropped";
static constexpr const char *descrMetricSystemNetworkDropped =
    "Count of packets that are dropped or discarded even though there was no error";
static constexpr const char *unitMetricSystemNetworkDropped = "{packet}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemNetworkDropped(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemNetworkDropped, descrMetricSystemNetworkDropped,
                                    unitMetricSystemNetworkDropped);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemNetworkDropped(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricSystemNetworkDropped, descrMetricSystemNetworkDropped, unitMetricSystemNetworkDropped);
}

/**
 * Count of network errors detected
 * <p>
 * Measured as:
 * <p>
 * <ul>
 *   <li>Linux: the @code errs @endcode column in @code /proc/dev/net @endcode (<a
 * href="https://web.archive.org/web/20180321091318/http://www.onlamp.com/pub/a/linux/2000/11/16/LinuxAdmin.html">source</a>).</li>
 *   <li>Windows: <a
 * href="https://docs.microsoft.com/windows/win32/api/netioapi/ns-netioapi-mib_if_row2">@code
 * InErrors @endcode/@code OutErrors @endcode</a> from <a
 * href="https://docs.microsoft.com/windows/win32/api/netioapi/nf-netioapi-getifentry2">@code
 * GetIfEntry2 @endcode</a>.</li>
 * </ul>
 * counter
 */
static constexpr const char *kMetricSystemNetworkErrors     = "metric.system.network.errors";
static constexpr const char *descrMetricSystemNetworkErrors = "Count of network errors detected";
static constexpr const char *unitMetricSystemNetworkErrors  = "{error}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemNetworkErrors(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemNetworkErrors, descrMetricSystemNetworkErrors,
                                    unitMetricSystemNetworkErrors);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemNetworkErrors(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricSystemNetworkErrors, descrMetricSystemNetworkErrors, unitMetricSystemNetworkErrors);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemNetworkIo     = "metric.system.network.io";
static constexpr const char *descrMetricSystemNetworkIo = "";
static constexpr const char *unitMetricSystemNetworkIo  = "By";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemNetworkIo, descrMetricSystemNetworkIo,
                                    unitMetricSystemNetworkIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemNetworkIo, descrMetricSystemNetworkIo,
                                             unitMetricSystemNetworkIo);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemNetworkPackets     = "metric.system.network.packets";
static constexpr const char *descrMetricSystemNetworkPackets = "";
static constexpr const char *unitMetricSystemNetworkPackets  = "{packet}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemNetworkPackets(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemNetworkPackets, descrMetricSystemNetworkPackets,
                                    unitMetricSystemNetworkPackets);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemNetworkPackets(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricSystemNetworkPackets, descrMetricSystemNetworkPackets, unitMetricSystemNetworkPackets);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemPagingFaults     = "metric.system.paging.faults";
static constexpr const char *descrMetricSystemPagingFaults = "";
static constexpr const char *unitMetricSystemPagingFaults  = "{fault}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemPagingFaults(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemPagingFaults, descrMetricSystemPagingFaults,
                                    unitMetricSystemPagingFaults);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemPagingFaults(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricSystemPagingFaults, descrMetricSystemPagingFaults, unitMetricSystemPagingFaults);
}

/**
 * counter
 */
static constexpr const char *kMetricSystemPagingOperations     = "metric.system.paging.operations";
static constexpr const char *descrMetricSystemPagingOperations = "";
static constexpr const char *unitMetricSystemPagingOperations  = "{operation}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemPagingOperations(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemPagingOperations,
                                    descrMetricSystemPagingOperations,
                                    unitMetricSystemPagingOperations);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemPagingOperations(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricSystemPagingOperations,
                                             descrMetricSystemPagingOperations,
                                             unitMetricSystemPagingOperations);
}

/**
 * Unix swap or windows pagefile usage
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricSystemPagingUsage     = "metric.system.paging.usage";
static constexpr const char *descrMetricSystemPagingUsage = "Unix swap or windows pagefile usage";
static constexpr const char *unitMetricSystemPagingUsage  = "By";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemPagingUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemPagingUsage, descrMetricSystemPagingUsage,
                                         unitMetricSystemPagingUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemPagingUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricSystemPagingUsage, descrMetricSystemPagingUsage, unitMetricSystemPagingUsage);
}

/**
 * gauge
 */
static constexpr const char *kMetricSystemPagingUtilization = "metric.system.paging.utilization";
static constexpr const char *descrMetricSystemPagingUtilization = "";
static constexpr const char *unitMetricSystemPagingUtilization  = "1";

static nostd::unique_ptr<metrics::Gauge<uint64_t>> CreateSyncMetricSystemPagingUtilization(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Gauge(kMetricSystemPagingUtilization,
                                  descrMetricSystemPagingUtilization,
                                  unitMetricSystemPagingUtilization);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemPagingUtilization(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableGauge(kMetricSystemPagingUtilization,
                                           descrMetricSystemPagingUtilization,
                                           unitMetricSystemPagingUtilization);
}

/**
 * Total number of processes in each state
 * <p>
 * updowncounter
 */
static constexpr const char *kMetricSystemProcessCount = "metric.system.process.count";
static constexpr const char *descrMetricSystemProcessCount =
    "Total number of processes in each state";
static constexpr const char *unitMetricSystemProcessCount = "{process}";

static nostd::unique_ptr<metrics::UpDownCounter<uint64_t>> CreateSyncMetricSystemProcessCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64UpDownCounter(kMetricSystemProcessCount, descrMetricSystemProcessCount,
                                         unitMetricSystemProcessCount);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemProcessCount(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableUpDownCounter(
      kMetricSystemProcessCount, descrMetricSystemProcessCount, unitMetricSystemProcessCount);
}

/**
 * Total number of processes created over uptime of the host
 * <p>
 * counter
 */
static constexpr const char *kMetricSystemProcessCreated = "metric.system.process.created";
static constexpr const char *descrMetricSystemProcessCreated =
    "Total number of processes created over uptime of the host";
static constexpr const char *unitMetricSystemProcessCreated = "{process}";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricSystemProcessCreated(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricSystemProcessCreated, descrMetricSystemProcessCreated,
                                    unitMetricSystemProcessCreated);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricSystemProcessCreated(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricSystemProcessCreated, descrMetricSystemProcessCreated, unitMetricSystemProcessCreated);
}

}  // namespace system
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
