// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW

#  include <memory>
#  include <mutex>
#  include "opentelemetry/common/spin_lock_mutex.h"
#  include "opentelemetry/exporters/memory/in_memory_metric_data.h"
#  include "opentelemetry/sdk/common/circular_buffer.h"
#  include "opentelemetry/sdk/metrics/data/metric_data.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/metric_exporter.h"
#  include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{
const size_t MAX_BUFFER_SIZE = 100;

/**
 * The in-memory metrics exporter exports metrics data to the memory.
 *
 */
class InMemoryMetricExporter final : public opentelemetry::sdk::metrics::MetricExporter
{
public:
  /**
   * Create an InMemoryMetricExporter. The constructor takes the buffer size as the input and
   * initializes the ring buffer with the given size.
   *
   */
  explicit InMemoryMetricExporter(size_t buffer_size = MAX_BUFFER_SIZE)
      : data_(new InMemoryMetricData(buffer_size))
  {}

  sdk::common::ExportResult Export(const sdk::metrics::ResourceMetrics &data) noexcept override;

  bool ForceFlush(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  bool Shutdown(
      std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept override;

  /**
   * @return Returns a shared pointer to this exporters InMemoryMetricData.
   */
  inline InMemoryMetricData &GetData() noexcept { return *data_; }

private:
  bool isShutdown() const noexcept;
  bool is_shutdown_ = false;
  mutable opentelemetry::common::SpinLockMutex lock_;

  std::unique_ptr<InMemoryMetricData> data_ = nullptr;
};
}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
#endif
