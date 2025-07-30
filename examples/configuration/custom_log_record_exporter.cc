// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/logs/read_write_log_record.h"
#include "opentelemetry/sdk/logs/recordable.h"

#include "custom_log_record_exporter.h"

std::unique_ptr<opentelemetry::sdk::logs::Recordable>
CustomLogRecordExporter::MakeRecordable() noexcept
{
  auto recordable = std::make_unique<opentelemetry::sdk::logs::ReadWriteLogRecord>();
  return recordable;
}

opentelemetry::sdk::common::ExportResult CustomLogRecordExporter::Export(
    const opentelemetry::nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>>
        & /* records */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomLogRecordExporter::Export(): YOUR CODE HERE");
  return opentelemetry::sdk::common::ExportResult::kSuccess;
}

bool CustomLogRecordExporter::ForceFlush(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomLogRecordExporter::ForceFlush(): YOUR CODE HERE");
  return false;
}

bool CustomLogRecordExporter::Shutdown(std::chrono::microseconds /* timeout */) noexcept
{
  OTEL_INTERNAL_LOG_ERROR("CustomLogRecordExporter::Shutdown(): YOUR CODE HERE");
  return false;
}
