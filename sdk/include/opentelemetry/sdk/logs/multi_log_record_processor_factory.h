// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifdef ENABLE_LOGS_PREVIEW

#  include <memory>
#  include <vector>

#  include "opentelemetry/sdk/logs/processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

/**
 * Factory class for MultiLogRecordProcessor.
 */
class MultiLogRecordProcessorFactory
{
public:
  static std::unique_ptr<LogRecordProcessor> Create(
      std::vector<std::unique_ptr<LogRecordProcessor>> &&processors);
};

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
