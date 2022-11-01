// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_LOGS_PREVIEW

#  include "opentelemetry/sdk/logs/batch_log_record_processor_factory.h"
#  include "opentelemetry/sdk/logs/batch_log_record_processor.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{

std::unique_ptr<LogRecordProcessor> BatchLogProcessorFactory::Create(
    std::unique_ptr<LogRecordExporter> &&exporter,
    const BatchLogProcessorOptions &options)
{
  std::unique_ptr<LogRecordProcessor> processor(
      new BatchLogProcessor(std::move(exporter), options));
  return processor;
}

}  // namespace logs
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
