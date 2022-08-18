// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/exporter.h"
#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for SimpleLogProcessor.
 */
class SimpleLogProcessorFactory
{
public:
  /**
   * Create a SimpleLogProcessor.
   */
  static std::unique_ptr<LogProcessor> Create(std::unique_ptr<LogExporter> &&exporter);
};

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif /* ENABLE_LOGS_PREVIEW */
