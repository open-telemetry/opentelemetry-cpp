// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/multi_log_processor_factory.h"
#  include "opentelemetry/sdk/logs/multi_log_processor.h"

#  include <chrono>
#  include <memory>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<LogProcessor> MultiLogProcessorFactory::Create(
    std::vector<std::unique_ptr<LogProcessor>> &&processors)
{
  std::unique_ptr<LogProcessor> processor(new MultiLogProcessor(std::move(processors)));
  return processor;
}

}  // namespace logs
}  // namespace sdk

OPENTELEMETRY_END_NAMESPACE
#endif
