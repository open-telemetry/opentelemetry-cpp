// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/simple_log_processor_factory.h"
#  include "opentelemetry/sdk/logs/simple_log_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<LogProcessor> SimpleLogProcessorFactory::Create(
    std::unique_ptr<LogExporter> &&exporter)
{
  std::unique_ptr<LogProcessor> processor(new SimpleLogProcessor(std::move(exporter)));
  return processor;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
