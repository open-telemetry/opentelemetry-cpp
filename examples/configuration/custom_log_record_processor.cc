// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <utility>

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"

#include "custom_log_record_processor.h"

std::unique_ptr<opentelemetry::sdk::logs::Recordable>
CustomLogRecordProcessor::MakeRecordable() noexcept
{
  auto recordable = std::make_unique<opentelemetry::sdk::logs::ReadWriteLogRecord>();
  return recordable;
}

void CustomLogRecordProcessor::OnEmit(
    std::unique_ptr<opentelemetry::sdk::logs::Recordable> &&span) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomLogRecordProcessor::OnEnd(): YOUR CODE HERE");
  auto unused = std::move(span);
}

bool CustomLogRecordProcessor::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomLogRecordProcessor::ForceFlush(): YOUR CODE HERE");
  return false;
}

bool CustomLogRecordProcessor::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomLogRecordProcessor::Shutdown(): YOUR CODE HERE");
  return false;
}
