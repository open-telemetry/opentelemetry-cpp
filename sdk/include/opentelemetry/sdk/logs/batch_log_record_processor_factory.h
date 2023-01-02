// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>

#  include "opentelemetry/sdk/logs/batch_log_record_processor_options.h"
#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace logs
{

/**
 * Factory class for BatchLogRecordProcessor.
 */
class BatchLogRecordProcessorFactory
{
public:
  /**
   * Create a BatchLogRecordProcessor.
   */
  static std::unique_ptr<LogRecordProcessor> Create(std::unique_ptr<LogRecordExporter> &&exporter,
                                                    const BatchLogRecordProcessorOptions &options);
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
