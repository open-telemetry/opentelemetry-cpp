// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/exporters/memory/in_memory_data.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace memory
{

class InMemoryMetricData final : public exporter::memory::InMemoryData<sdk::metrics::MetricData>
{
public:
  /**
   * @param buffer_size a required value that sets the size of the CircularBuffer
   */
  explicit InMemoryMetricData(size_t buffer_size)
      : exporter::memory::InMemoryData<sdk::metrics::MetricData>(buffer_size)
  {}

  std::vector<std::unique_ptr<sdk::metrics::MetricData>> GetMetrics() noexcept { return Get(); }
};
}  // namespace memory
}  // namespace exporter
