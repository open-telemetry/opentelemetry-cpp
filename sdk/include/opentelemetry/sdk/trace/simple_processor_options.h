// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace trace
{

/**
 * Struct to hold simple SpanProcessor options.
 */
struct SimpleSpanProcessorOptions
{
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