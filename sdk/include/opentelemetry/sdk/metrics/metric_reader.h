// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/sdk/metrics/export/metric_producer.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
/**
 * Configuration options for cardinality limits per instrument type.
 * A value of 0 means use the SDK default (2000).
 */
struct CardinalityLimitOptions
{
  size_t default_limit              = 0;  // 0 means use SDK default (2000)
  size_t counter                    = 0;
  size_t gauge                      = 0;
  size_t histogram                  = 0;
  size_t observable_counter         = 0;
  size_t observable_gauge           = 0;
  size_t observable_up_down_counter = 0;
  size_t up_down_counter            = 0;
};

/**
 * MetricReader defines the interface to collect metrics from SDK
 */
class MetricReader
{
public:
  MetricReader()                                = default;
  MetricReader(const MetricReader &)            = delete;
  MetricReader(MetricReader &&)                 = delete;
  MetricReader &operator=(const MetricReader &) = delete;
  MetricReader &operator=(MetricReader &&)      = delete;

  void SetMetricProducer(MetricProducer *metric_producer);

  /**
   * Collect the metrics from SDK.
   * @return return the status of the operation.
   */
  bool Collect(nostd::function_ref<bool(ResourceMetrics &metric_data)> callback) noexcept;

  /**
   * Get the AggregationTemporality for given Instrument Type for this reader.
   *
   * @return AggregationTemporality
   */

  virtual AggregationTemporality GetAggregationTemporality(
      InstrumentType instrument_type) const noexcept = 0;

  /**
   * Get the cardinality limit for a given instrument type.
   * Returns 0 if no limit is configured for this instrument type.
   *
   * @param instrument_type The instrument type to get the limit for
   * @return The cardinality limit, or 0 if not configured
   */
  size_t GetCardinalityLimit(InstrumentType instrument_type) const noexcept;

  /**
   * Set cardinality limit options for this reader.
   *
   * @param options The cardinality limit options
   */
  void SetCardinalityLimitOptions(const CardinalityLimitOptions &options) noexcept;

  /**
   * Shutdown the metric reader.
   */
  bool Shutdown(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

  /**
   * Force flush the metric read by the reader.
   */
  bool ForceFlush(std::chrono::microseconds timeout = (std::chrono::microseconds::max)()) noexcept;

  /**
   * Return the status of Metric reader.
   */
  bool IsShutdown() const noexcept;

  virtual ~MetricReader() = default;

private:
  virtual bool OnForceFlush(std::chrono::microseconds timeout) noexcept = 0;

  virtual bool OnShutDown(std::chrono::microseconds timeout) noexcept = 0;

  virtual void OnInitialized() noexcept {}

protected:
private:
  MetricProducer *metric_producer_{nullptr};
  std::atomic<bool> shutdown_{false};
  CardinalityLimitOptions cardinality_limit_options_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
