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
namespace container
{

/**
 * Total CPU time consumed
 * <p>
 * Total CPU time consumed by the specific container on all available CPU cores
 * <p>
 * counter
 */
static constexpr const char *kMetricContainerCpuTime     = "metric.container.cpu.time";
static constexpr const char *descrMetricContainerCpuTime = "Total CPU time consumed";
static constexpr const char *unitMetricContainerCpuTime  = "s";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricContainerCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricContainerCpuTime, descrMetricContainerCpuTime,
                                    unitMetricContainerCpuTime);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricContainerCpuTime(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricContainerCpuTime, descrMetricContainerCpuTime,
                                             unitMetricContainerCpuTime);
}

/**
 * Disk bytes for the container.
 * <p>
 * The total number of bytes read/written successfully (aggregated from all disks).
 * <p>
 * counter
 */
static constexpr const char *kMetricContainerDiskIo     = "metric.container.disk.io";
static constexpr const char *descrMetricContainerDiskIo = "Disk bytes for the container.";
static constexpr const char *unitMetricContainerDiskIo  = "By";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricContainerDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricContainerDiskIo, descrMetricContainerDiskIo,
                                    unitMetricContainerDiskIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricContainerDiskIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(kMetricContainerDiskIo, descrMetricContainerDiskIo,
                                             unitMetricContainerDiskIo);
}

/**
 * Memory usage of the container.
 * <p>
 * Memory usage of the container.
 * <p>
 * counter
 */
static constexpr const char *kMetricContainerMemoryUsage     = "metric.container.memory.usage";
static constexpr const char *descrMetricContainerMemoryUsage = "Memory usage of the container.";
static constexpr const char *unitMetricContainerMemoryUsage  = "By";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricContainerMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricContainerMemoryUsage, descrMetricContainerMemoryUsage,
                                    unitMetricContainerMemoryUsage);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricContainerMemoryUsage(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricContainerMemoryUsage, descrMetricContainerMemoryUsage, unitMetricContainerMemoryUsage);
}

/**
 * Network bytes for the container.
 * <p>
 * The number of bytes sent/received on all network interfaces by the container.
 * <p>
 * counter
 */
static constexpr const char *kMetricContainerNetworkIo     = "metric.container.network.io";
static constexpr const char *descrMetricContainerNetworkIo = "Network bytes for the container.";
static constexpr const char *unitMetricContainerNetworkIo  = "By";

static nostd::unique_ptr<metrics::Counter<uint64_t>> CreateSyncMetricContainerNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateUInt64Counter(kMetricContainerNetworkIo, descrMetricContainerNetworkIo,
                                    unitMetricContainerNetworkIo);
}

static nostd::shared_ptr<metrics::ObservableInstrument> CreateAsyncMetricContainerNetworkIo(
    metrics::Meter *meter)
{
  return meter->CreateInt64ObservableCounter(
      kMetricContainerNetworkIo, descrMetricContainerNetworkIo, unitMetricContainerNetworkIo);
}

}  // namespace container
}  // namespace semconv
OPENTELEMETRY_END_NAMESPACE
