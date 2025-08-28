// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace trace
{

/**
 * Struct to hold batch SpanProcessor options.
 */
struct BatchSpanProcessorOptions
{
  /**
   * The maximum buffer/queue size. After the size is reached, spans are
   * dropped.
   */
  size_t max_queue_size = 2048;

  /* The time interval between two consecutive exports. */
  std::chrono::milliseconds schedule_delay_millis = std::chrono::milliseconds(5000);

  /**
   * The maximum batch size of every export. It must be smaller or
   * equal to max_queue_size.
   */
  size_t max_export_batch_size = 512;

  /**
   * Whether to export unsampled but recording spans.
   * By default, only sampled spans (Decision::RECORD_AND_SAMPLE) are exported.
   * When set to true, unsampled recording spans (Decision::RECORD_ONLY) are also exported.
   */
  bool export_unsampled_spans = false;
};

}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
