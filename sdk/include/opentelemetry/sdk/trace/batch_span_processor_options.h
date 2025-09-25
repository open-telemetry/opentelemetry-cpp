// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

#include "opentelemetry/sdk/common/env_variables.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace trace
{

constexpr const char *kMaxQueueSize       = "OTEL_BSP_MAX_QUEUE_SIZE";
constexpr const char *kScheduleDelay      = "OTEL_BSP_SCHEDULE_DELAY";
constexpr const char *kExportTimeout      = "OTEL_BSP_EXPORT_TIMEOUT";
constexpr const char *kMaxExportBatchSize = "OTEL_BSP_MAX_EXPORT_BATCH_SIZE";

// The default max queue size is 2084
const size_t kDefaultMaxQueueSize = 2084;
// It is the default delay between two consecutive exports and default is 5000 milliseconds
const std::chrono::milliseconds kDefaultScheduleDelayMillis = std::chrono::milliseconds(5000);
// It is the default duration after which an export is cancelled and default is 3000 milliseconds.
const std::chrono::milliseconds kDefaultExportTimeout = std::chrono::milliseconds(3000);
// It is the default max batch size of every export and it is 512
const size_t kDefaultMaxExportBatchSize = 512;

/**
 * Reads the maximum queue size from the environment variable OTEL_BSP_MAX_QUEUE_SIZE.
 * If the environment variable is not set, returns the default value kDefaultMaxQueueSize.
 * This value determines the maximum number of spans that can be buffered before dropping.
 */
inline size_t GetMaxQueueSizeFromEnv()
{
  std::uint32_t max_queue_size;
  bool exists =
      opentelemetry::sdk::common::GetUintEnvironmentVariable(kMaxQueueSize, max_queue_size);
  if (exists == false)
  {
    return kDefaultMaxQueueSize;
  }
  return static_cast<size_t>(max_queue_size);
}

/**
 * Reads a duration from the specified environment variable.
 * If the variable is not set, returns the provided default_duration.
 * Converts the returned duration to milliseconds regardless of the original unit.
 * Used for schedule delays and export timeouts.
 */
inline std::chrono::milliseconds GetDurationFromEnv(
    const char *env_var,
    const std::chrono::milliseconds &default_duration)
{
  std::chrono::system_clock::duration duration{0};
  bool exists = opentelemetry::sdk::common::GetDurationEnvironmentVariable(env_var, duration);
  if (exists == false)
  {
    return default_duration;
  }
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

/**
 * Reads the maximum export batch size from the environment variable OTEL_BSP_MAX_EXPORT_BATCH_SIZE.
 * If the environment variable is not set, returns the default value kDefaultMaxExportBatchSize.
 * This determines the maximum number of spans exported in a single batch.
 */
inline size_t GetMaxExportBatchSizeFromEnv()
{
  std::uint32_t max_export_batch_size;
  bool exists = opentelemetry::sdk::common::GetUintEnvironmentVariable(kMaxExportBatchSize,
                                                                       max_export_batch_size);
  if (exists == false)
  {
    return kDefaultMaxExportBatchSize;
  }
  return static_cast<size_t>(max_export_batch_size);
}

/**
 * Struct to hold batch SpanProcessor options.
 */
struct BatchSpanProcessorOptions
{
  /**
   * The maximum buffer/queue size. After the size is reached, spans are
   * dropped.
   */
  size_t max_queue_size = GetMaxQueueSizeFromEnv();

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds schedule_delay_millis =
      GetDurationFromEnv(kScheduleDelay, kDefaultScheduleDelayMillis);

  /**
   * The maximum time allowed to to export data
   */
  std::chrono::milliseconds export_timeout =
      GetDurationFromEnv(kExportTimeout, kDefaultExportTimeout);

  /**
   * The maximum batch size of every export. It must be smaller or
   * equal to max_queue_size.
   */
  size_t max_export_batch_size = GetMaxExportBatchSizeFromEnv();
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
