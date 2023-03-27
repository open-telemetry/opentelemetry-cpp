// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>

#  include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{

namespace logs
{

struct BatchLogRecordProcessorOptions;
class LogRecordExporter;
class LogRecordProcessor;

/**
 * Factory class for BatchLogRecordProcessor.
 */
class OPENTELEMETRY_EXPORT BatchLogRecordProcessorFactory
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
