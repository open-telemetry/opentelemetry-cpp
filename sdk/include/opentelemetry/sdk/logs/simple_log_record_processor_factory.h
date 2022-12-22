// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>

#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for SimpleLogRecordProcessor.
 */
class SimpleLogRecordProcessorFactory
{
public:
  /**
   * Create a SimpleLogRecordProcessor.
   */
  static std::unique_ptr<LogRecordProcessor> Create(std::unique_ptr<LogRecordExporter> &&exporter);
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif /* ENABLE_LOGS_PREVIEW */
