// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/batch_log_processor_options.h"
#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace logs
{

/**
 * Factory class for BatchLogProcessor.
 */
class BatchLogProcessorFactory
{
public:
  /**
   * Create a BatchLogProcessor.
   */
  std::unique_ptr<LogProcessor> Create(std::unique_ptr<LogExporter> &&exporter,
                                       const BatchLogProcessorOptions &options);
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
