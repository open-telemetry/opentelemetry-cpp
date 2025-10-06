// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <cstddef>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace logs
{

/**
 * Struct to hold batch LogRecordProcessor options.
 */
struct OPENTELEMETRY_EXPORT BatchLogRecordProcessorOptions
{
  BatchLogRecordProcessorOptions();
  /**
   * The maximum buffer/queue size. After the size is reached, spans are
   * dropped.
   */
  size_t max_queue_size;

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds schedule_delay_millis;

  /**
   * It is the time duration of how long the export can run before it is cancelled
   * It is not currently used by the SDK and the parameter is ignored
   * TODO: Implement the parameter in BatchLogRecordProcessor
   */
  std::chrono::milliseconds export_timeout_millis;

  /**
   * The maximum batch size of every export. It must be smaller or
   * equal to max_queue_size.
   */
  size_t max_export_batch_size;
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
