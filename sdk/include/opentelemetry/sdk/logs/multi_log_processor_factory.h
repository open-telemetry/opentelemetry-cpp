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
 * Factory class for MultiLogProcessor.
 */
class MultiLogProcessorFactory
{
public:
  static std::unique_ptr<LogProcessor> Create(
      std::vector<std::unique_ptr<LogProcessor>> &&processors);
};

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
