// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace trace
{

namespace batch_span_processor_options_env
{

inline size_t GetMaxQueueSizeFromEnv()
{
  constexpr size_t kDefaultMaxQueueSize = 2048;
  std::uint32_t value{};
  if (!opentelemetry::sdk::common::GetUintEnvironmentVariable("OTEL_BSP_MAX_QUEUE_SIZE", value))
  {
    return kDefaultMaxQueueSize;
  }
  return static_cast<size_t>(value);
}

inline std::chrono::milliseconds GetScheduleDelayFromEnv()
{
  const std::chrono::milliseconds kDefaultScheduleDelay{5000};
  std::chrono::system_clock::duration duration{0};
  if (!opentelemetry::sdk::common::GetDurationEnvironmentVariable("OTEL_BSP_SCHEDULE_DELAY",
                                                                  duration))
  {
    return kDefaultScheduleDelay;
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

inline std::chrono::milliseconds GetExportTimeoutFromEnv()
{
  const std::chrono::milliseconds kDefaultExportTimeout{30000};
  std::chrono::system_clock::duration duration{0};
  if (!opentelemetry::sdk::common::GetDurationEnvironmentVariable("OTEL_BSP_EXPORT_TIMEOUT",
                                                                  duration))
  {
    return kDefaultExportTimeout;
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

inline size_t GetMaxExportBatchSizeFromEnv()
{
  constexpr size_t kDefaultMaxExportBatchSize = 512;
  std::uint32_t value{};
  if (!opentelemetry::sdk::common::GetUintEnvironmentVariable("OTEL_BSP_MAX_EXPORT_BATCH_SIZE",
                                                              value))
  {
    return kDefaultMaxExportBatchSize;
  }
  return static_cast<size_t>(value);
}

}  // namespace batch_span_processor_options_env

/**
 * Struct to hold batch SpanProcessor options.
 */
struct OPENTELEMETRY_EXPORT BatchSpanProcessorOptions
{
  /**
   * The maximum buffer/queue size. After the size is reached, spans are
   * dropped.
   */
  size_t max_queue_size = batch_span_processor_options_env::GetMaxQueueSizeFromEnv();

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds schedule_delay_millis =
      batch_span_processor_options_env::GetScheduleDelayFromEnv();

  /**
   * The maximum time allowed to to export data
   * It is not currently used by the SDK and the parameter is ignored
   * TODO: Implement the parameter in BatchSpanProcessor
   */
  std::chrono::milliseconds export_timeout =
      batch_span_processor_options_env::GetExportTimeoutFromEnv();

  /**
   * The maximum batch size of every export. It must be smaller or
   * equal to max_queue_size.
   */
  size_t max_export_batch_size = batch_span_processor_options_env::GetMaxExportBatchSizeFromEnv();
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
